#!/usr/bin/env python3
"""Name unnamed overlay functions after their exact twins that already have a name.

Usage:  tools/propagate_names.py [--tsv FILE] [--min-size 0x24] [--from-stcom] [--apply]

Reads the table tools/dupe_scan.py writes (`dupe_scan.py --tsv logs/scratch/dupes.tsv`) and the
overlay registries (config/r3_*/symbol_addrs.txt). For each exact group (same instructions and
immediates, own addresses masked) that holds at least one named function, every unnamed member
in a stage overlay gets the twin's name with the overlay prefix: ROCK_NEO's calc_distance copied
into ST05 becomes st05_calc_distance. Names are lower-cased after the prefix.

Left out, and listed in the report:
- conflict: the named members disagree (after removing any stNN_/sup_ prefix), as small generic
  wrappers do (Title_chr_close and Effect_explosion_end share bytes);
- generic: the twin's name says where it is used, not what it does (GENERIC_SOURCES);
- stage_common: the only names are stcomNN_ ones, so the function is probably a stage_common copy
  this overlay doesn't link yet; that is a split question, not a naming one. Once tools/link_stcom.py
  has linked the whole-file copies, --from-stcom names the rest (stage variants that share some
  functions with a stcom file) after the stcom function: stcom16_look_turn in ST1D ->
  st1d_stcom16_look_turn;
- collision: the new name is taken in that overlay, or two functions of one overlay would get it
  (identical copies in two of its files);
- small: under --min-size bytes (default 0x24). A shared body says little there, and the 0x20
  one-call wrappers (Title_chr_close is Close_EM_WORK(wp)) would carry a name from another context.

Without --apply it writes logs/scratch/propagate/<registry>.txt (pairs "OLD NEW", one file per
overlay) and logs/scratch/propagate/report.txt, and changes nothing.

With --apply it also, per overlay: adds "NEW = 0xADDR; // type:func" to the overlay's registry and
replaces OLD with NEW (whole words) in src/<registry>/ and include/<registry>/ only. Overlay func_
names repeat between overlays (they all load at 0x80100000), so a repo-wide sweep would rename
other overlays' functions; this is why the replacement is scoped. Then run
`make split-overlays overlays` and the usual MATCH check.
"""

import argparse
import csv
import re
import sys
from collections import Counter, defaultdict
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
EXECUTABLES = {"ROCK_NEO", "SLUS"}
SYM_RE = re.compile(r"\s*([A-Za-z_]\w*)\s*=\s*0x([0-9A-Fa-f]+)")
UNNAMED_RE = re.compile(r"^(func_[0-9A-Fa-f]+|stcom\d+_func_[0-9A-Fa-f]+)$")
PREFIX_RE = re.compile(r"^(st[0-9a-f]{2}|shl[0-9a-f]{2}|sup)_")
STCOM_RE = re.compile(r"^stcom\d+_")
FUNC_WORD = re.compile(r"\bfunc_[0-9A-Fa-f]{8}\b")
# Twins whose name describes where they are used, not what the body does: the copy in another
# overlay would get a wrong name. Effect_debris_wait2 is a bare "count down, then next routine".
GENERIC_SOURCES = {"Effect_debris_wait2"}


def registry_of(prog):
    """Config folder of a program from dupe_scan, or None for one this repo doesn't split."""
    name = "r3_sup" if prog == "SUPPORT" else "r3_" + prog.lower()
    return name if (ROOT / "config" / name / "symbol_addrs.txt").exists() else None


def prefix_of(registry):
    return registry[3:]


def is_named(name):
    return bool(name) and not UNNAMED_RE.match(name)


def load_registry(registry):
    """address -> name, and the set of names, of one overlay registry."""
    by_addr = {}
    names = set()
    path = ROOT / "config" / registry / "symbol_addrs.txt"
    for line in path.read_text(encoding="utf-8").splitlines():
        m = SYM_RE.match(line)
        if m:
            by_addr[int(m.group(2), 16)] = m.group(1)
            names.add(m.group(1))
    return by_addr, names


def source_files(registry):
    for top in ("src", "include"):
        folder = ROOT / top / registry
        if folder.is_dir():
            yield from sorted(p for p in folder.rglob("*") if p.suffix in (".c", ".h"))


def words_in_sources(registry):
    found = set()
    for path in source_files(registry):
        found.update(re.findall(r"\b[A-Za-z_]\w*\b", path.read_text(encoding="utf-8")))
    return found


def main():
    ap = argparse.ArgumentParser(description=__doc__.split("\n")[0])
    ap.add_argument("--tsv", default="logs/scratch/dupes.tsv")
    ap.add_argument("--min-size", type=lambda s: int(s, 0), default=0x24)
    ap.add_argument("--apply", action="store_true")
    ap.add_argument("--from-stcom", action="store_true",
                    help="also name twins whose only names are stcomNN_ ones (stage variants of "
                    "a stcom file, named st1d_stcom16_look_turn; link whole-file copies first)")
    args = ap.parse_args()

    tsv = ROOT / args.tsv
    if not tsv.exists():
        sys.exit(f"{args.tsv} not found: run tools/dupe_scan.py --tsv {args.tsv} first")
    rows = list(csv.DictReader(tsv.open(encoding="utf-8"), delimiter="\t"))

    registries = {}
    for prog in {r["prog"] for r in rows} - EXECUTABLES:
        reg = registry_of(prog)
        if reg:
            registries[prog] = (reg, *load_registry(reg))

    def name_of(r):
        if r["prog"] in EXECUTABLES:
            return r["name"]
        if r["prog"] not in registries:
            return ""
        return registries[r["prog"]][1].get(int(r["addr"], 16), "")

    groups = defaultdict(list)
    for r in rows:
        groups[r["exact_group"]].append(r)

    proposals = defaultdict(list)  # registry -> [(old, new, addr, source)]
    skipped = defaultdict(list)  # reason -> lines
    for members in groups.values():
        named = sorted({name_of(r) for r in members if is_named(name_of(r))})
        if not named:
            continue
        targets = [r for r in members if r["prog"] in registries and not is_named(name_of(r))]
        if not targets:
            continue
        where = ", ".join(f"{r['prog']}:{r['addr']}" for r in targets)
        plain = [n for n in named if not STCOM_RE.match(n)]
        if set(plain) & GENERIC_SOURCES:
            skipped["generic"].append(f"{where} = {', '.join(plain)}")
            continue
        if not plain and not args.from_stcom:
            skipped["stage_common"].append(f"{where} = {', '.join(named)}")
            continue
        if not plain:
            # --from-stcom: a stage's variant of a stcom file shares these functions; name them
            # after the stcom function, keeping its stcom so the variant is visible
            # (stcom16_look_turn in ST1D -> st1d_stcom16_look_turn; the owner's choice).
            plain = named
        bases = sorted({PREFIX_RE.sub("", n).lower() for n in plain})
        if len(bases) > 1:
            skipped["conflict"].append(f"{where} = {', '.join(plain)}")
            continue
        for r in targets:
            size = int(r["size"], 16)
            reg = registries[r["prog"]][0]
            old = f"func_{int(r['addr'], 16):08X}"
            new = f"{prefix_of(reg)}_{bases[0]}"
            line = f"{r['prog']}:{r['addr']} {old} -> {new} (from {', '.join(plain)})"
            if size < args.min_size:
                skipped["small"].append(f"{line}, 0x{size:x} bytes")
                continue
            proposals[reg].append((old, new, int(r["addr"], 16), line))

    # Collisions: a name taken in the overlay, or given twice within it.
    final = {}
    for reg, items in proposals.items():
        prog = next(p for p, v in registries.items() if v[0] == reg)
        taken = registries[prog][2] | words_in_sources(reg)
        counts = Counter(new for _, new, _, _ in items)
        keep = []
        for old, new, addr, line in items:
            if counts[new] > 1:
                skipped["collision"].append(f"{line}: {counts[new]} functions of this overlay")
            elif new in taken:
                skipped["collision"].append(f"{line}: name already used")
            else:
                keep.append((old, new, addr, line))
        if keep:
            final[reg] = sorted(keep, key=lambda t: t[2])

    out = ROOT / "logs/scratch/propagate"
    out.mkdir(parents=True, exist_ok=True)
    for old_file in out.glob("r3_*.txt"):
        old_file.unlink()
    for reg, items in final.items():
        (out / f"{reg}.txt").write_text("".join(f"{o} {n}\n" for o, n, _, _ in items))
    report = [f"proposed: {sum(len(v) for v in final.values())} in {len(final)} overlays"]
    for reg in sorted(final):
        report.append(f"\n[{reg}]")
        report += [f"  {line}" for *_, line in final[reg]]
    for reason in ("conflict", "generic", "stage_common", "collision", "small"):
        report.append(f"\nskipped, {reason}: {len(skipped[reason])}")
        report += [f"  {s}" for s in sorted(skipped[reason])]
    (out / "report.txt").write_text("\n".join(report) + "\n", encoding="utf-8")

    print(report[0])
    for reason in ("conflict", "generic", "stage_common", "collision", "small"):
        print(f"skipped, {reason}: {len(skipped[reason])}")
    print(f"pairs and report in {out.relative_to(ROOT)}/")

    if not args.apply:
        return
    changed = 0
    for reg, items in final.items():
        mapping = {o: n for o, n, _, _ in items}
        reg_path = ROOT / "config" / reg / "symbol_addrs.txt"
        text = reg_path.read_text(encoding="utf-8")
        if not text.endswith("\n"):
            text += "\n"
        text += f"\n// Named after exact twins (tools/propagate_names.py).\n"
        text += "".join(f"{n} = 0x{a:08X}; // type:func\n" for _, n, a, _ in items)
        reg_path.write_text(text, encoding="utf-8", newline="\n")
        for path in source_files(reg):
            src = path.read_text(encoding="utf-8")
            new = FUNC_WORD.sub(lambda m: mapping.get(m.group(0), m.group(0)), src)
            if new != src:
                path.write_text(new, encoding="utf-8", newline="\n")
                changed += 1
    left = 0
    for reg, items in final.items():
        olds = {o for o, _, _, _ in items}
        for path in source_files(reg):
            left += len(olds & set(FUNC_WORD.findall(path.read_text(encoding="utf-8"))))
    print(f"applied: {changed} source files changed, {left} old names left")
    if left:
        sys.exit(1)


if __name__ == "__main__":
    main()
