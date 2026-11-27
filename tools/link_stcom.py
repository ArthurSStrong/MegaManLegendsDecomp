#!/usr/bin/env python3
"""Link a stage_common file into an overlay that holds its own copy of that code.

Usage:  tools/link_stcom.py STAGE STCOM REF --at OFF --data ADDR [--rodata-at OFF]
                            [--host-rodata-split N] [--apply]

  STAGE   the overlay with the copy (r3_st0b)
  STCOM   the stage_common file (stcom18)
  REF     an overlay that already links it (r3_st05); its registry gives the names
  --at    the copy's code offset in STAGE (0xF498)
  --data  the address of the copy's first data symbol (the one at REF's lowest stcomNN_ data
          address): the data keeps REF's layout
  --rodata-at  the copy's .rodata offset, when STCOM has a .rodata segment in REF
  --host-rodata-split  the host file's .rodata bytes that belong to the code before the copy;
          the rest goes to the new file after it (only when the host file has .rodata and the
          copy sits in its middle)

The copy must be the whole file (tools/dupe_scan.py: the same functions, the same sizes, one
after another). Without --apply it prints the plan and checks it; with --apply it:
- adds the stcomNN_ names at the copy's addresses to config/STAGE/symbol_addrs.txt;
- adds the STCOM segment(s) to config/STAGE/STAGE.yaml, and a segment for the host file's code
  after the copy (named after its offset, r3_st17_10C7C);
- removes the copy's functions from the host file, moving what follows them to that new file (the
  host file itself goes when the copy starts it);
- removes the declarations of the moved func_/D_ names from include/STAGE/*.h.
Then update the "linked into N stages" comment of src/stage_common/STCOM.c, move any tools/cc.sh
case of the host file, and run `make split-overlays overlays` and the MATCH check.
"""

import argparse
import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
SYM_RE = re.compile(r"^([A-Za-z_]\w*) = 0x([0-9A-Fa-f]+);(.*)$")
SEG_RE = re.compile(r"^(\s*)- (?:\[0x([0-9A-Fa-f]+), (\.?\w+), (\w+)\]|\{start: 0x([0-9A-Fa-f]+), "
                    r"type: (\.?\w+), name: (\w+)(, dir: [^}]+)?\})(.*)$")
FUNC_DEF = re.compile(r"^(?:static )?[A-Za-z_][\w \*]*?\b(func_[0-9A-F]{8})\(", re.M)


def segments(yaml_text):
    """[(line index, offset, type, name, indent)] of the subsegments."""
    out = []
    for i, line in enumerate(yaml_text.split("\n")):
        m = SEG_RE.match(line)
        if m:
            if m.group(2):
                out.append((i, int(m.group(2), 16), m.group(3), m.group(4), m.group(1)))
            else:
                out.append((i, int(m.group(5), 16), m.group(6), m.group(7), m.group(1)))
    return out


def vram_of(yaml_text):
    return int(re.search(r"vram: 0x([0-9A-Fa-f]+)", yaml_text).group(1), 16)


def end_of(segs, i, typ_set):
    """Offset where the segment after segs[i] (of a type in typ_set) starts."""
    for s in segs[segs.index(i) + 1:]:
        if s[2] in typ_set:
            return s[1]
    return None


def split_c(text):
    """(preamble, [(name or None, chunk)]) with each chunk one top-level definition and the
    comments and blank lines before it."""
    lines = text.split("\n")
    starts = []
    for i, line in enumerate(lines):
        if line and not line[0].isspace() and line[0] not in "#/{}" and "(" in line \
                and not line.rstrip().endswith(";") and not line.startswith("typedef"):
            j = i
            while j > 0 and lines[j - 1].startswith("//"):
                j -= 1
            starts.append(j)
    if not starts:
        return text, []
    pre = "\n".join(lines[:starts[0]]).rstrip("\n") + "\n\n"
    chunks = []
    for a, b in zip(starts, starts[1:] + [len(lines)]):
        chunk = "\n".join(lines[a:b])
        m = FUNC_DEF.search(chunk)
        chunks.append((m.group(1) if m else None, chunk))
    return pre, chunks


def main():
    ap = argparse.ArgumentParser(description=__doc__.split("\n")[0])
    ap.add_argument("stage")
    ap.add_argument("stcom")
    ap.add_argument("ref")
    ap.add_argument("--at", type=lambda s: int(s, 0), required=True)
    ap.add_argument("--data", type=lambda s: int(s, 0))
    ap.add_argument("--rodata-at", type=lambda s: int(s, 0))
    ap.add_argument("--host-rodata-split", type=lambda s: int(s, 0))
    ap.add_argument("--apply", action="store_true")
    a = ap.parse_args()

    ref_yaml = (ROOT / f"config/{a.ref}/{a.ref}.yaml").read_text(encoding="utf-8")
    yaml_path = ROOT / f"config/{a.stage}/{a.stage}.yaml"
    yaml = yaml_path.read_text(encoding="utf-8")
    rsegs, segs = segments(ref_yaml), segments(yaml)
    rvram, vram = vram_of(ref_yaml), vram_of(yaml)

    rc = next(s for s in rsegs if s[2] == "c" and s[3] == a.stcom)
    size = end_of(rsegs, rc, {"c", "asm", "data"}) - rc[1]
    rr = next((s for s in rsegs if s[2] == ".rodata" and s[3] == a.stcom), None)
    if rr and a.rodata_at is None:
        sys.exit(f"{a.stcom} has .rodata in {a.ref}: give --rodata-at")

    # Names from the reference registry.
    ref_reg = (ROOT / f"config/{a.ref}/symbol_addrs.txt").read_text(encoding="utf-8")
    syms = []
    for line in ref_reg.split("\n"):
        m = SYM_RE.match(line)
        if m and m.group(1).startswith(a.stcom + "_"):
            syms.append((m.group(1), int(m.group(2), 16), m.group(3)))
    code_lo, code_hi = rvram + rc[1], rvram + rc[1] + size
    rod_lo = rvram + rr[1] if rr else None
    data = [s for s in syms if not (code_lo <= s[1] < code_hi)
            and not (rr and rod_lo <= s[1] < rod_lo + 0x10000 and s[1] < rvram + 0x1000)]
    data_lo = min((s[1] for s in data), default=None)
    if data and a.data is None:
        sys.exit(f"{a.stcom} has data in {a.ref} (first at 0x{data_lo:08X}): give --data")

    new_syms = []
    for name, addr, rest in syms:
        if code_lo <= addr < code_hi:
            new_syms.append((name, vram + a.at + addr - code_lo, rest))
        elif rr and addr < rvram + 0x1000:
            new_syms.append((name, vram + a.rodata_at + addr - rod_lo, rest))
        else:
            new_syms.append((name, a.data + addr - data_lo, rest))

    # The host file.
    host = [s for s in segs if s[2] == "c" and s[1] <= a.at][-1]
    host_end = end_of(segs, host, {"c", "asm", "data"})
    if a.at + size > host_end:
        sys.exit(f"the copy (0x{a.at:X}..0x{a.at + size:X}) runs past {host[3]} (0x{host_end:X})")
    src_dir = ROOT / f"src/{a.stage}"
    host_c = src_dir / f"{host[3]}.c"
    pre, chunks = split_c(host_c.read_text(encoding="utf-8"))
    lo, hi = vram + a.at, vram + a.at + size
    def addr(n):
        return int(n[5:], 16) if n else None
    before = [c for n, c in chunks if n is None or addr(n) < lo]
    moved = [n for n, c in chunks if n and lo <= addr(n) < hi]
    after = [c for n, c in chunks if n and addr(n) >= hi]
    funcs = sorted(s[1] for s in new_syms if "type:func" in s[2])
    if [addr(n) for n in moved] != funcs:
        sys.exit("the host file's functions in the copy don't match the reference's:\n"
                 f"  host: {[hex(addr(n)) for n in moved]}\n  ref:  {[hex(x) for x in funcs]}")
    stray = [n for n, c in chunks if n is None]
    if stray:
        print(f"note: {len(stray)} definitions that aren't func_ stay before the copy")
    after_name = f"{a.stage}_{a.at + size:X}" if after else None
    host_rod = next((s for s in segs if s[2] == ".rodata" and s[3] == host[3]), None)
    if host_rod and before and after and a.host_rodata_split is None:
        sys.exit(f"{host[3]} has .rodata and code on both sides: give --host-rodata-split")

    # Data the copy uses: every D_ it references should get a name or be ROCK_NEO's.
    copy_text = "\n".join(c for n, c in chunks if n and lo <= addr(n) < hi)
    used = sorted(set(re.findall(r"\bD_([0-9A-F]{8})\b", copy_text)))
    mapped = {s[1] for s in new_syms}
    own = [u for u in used if int(u, 16) >= vram and int(u, 16) not in mapped]
    renames = {f"func_{s[1]:08X}": s[0] for s in new_syms if "type:func" in s[2]}
    renames.update({f"D_{s[1]:08X}": s[0] for s in new_syms if "type:func" not in s[2]})

    print(f"{a.stcom} ({size:#x} bytes) into {a.stage} at {a.at:#x}, host {host[3]}")
    print(f"  {len(new_syms)} names; {len(moved)} functions leave {host[3]}.c")
    print(f"  before: {len(before)} definitions stay; after: {len(after)} go to "
          f"{after_name or '-'}{'.c' if after_name else ''}")
    if not before:
        print(f"  {host[3]}.c goes (the copy starts it)")
    if own:
        print(f"  WARNING: the copy uses own data that gets no name: {', '.join(own)}")
    if not a.apply:
        return

    # Registry.
    reg_path = ROOT / f"config/{a.stage}/symbol_addrs.txt"
    reg = reg_path.read_text(encoding="utf-8")
    for name, _, _ in new_syms:
        if re.search(rf"^{name} =", reg, re.M):
            sys.exit(f"{name} is already in {reg_path.relative_to(ROOT)}")
    # Entries the registry already has at those addresses (func_ names declared by hand) would
    # clash with the new names in splat.
    taken = {x for _, x, _ in new_syms}
    reg = "\n".join(l for l in reg.split("\n")
                    if not ((m := SYM_RE.match(l)) and int(m.group(2), 16) in taken))
    reg = reg.rstrip("\n") + f"\n\n// {a.stcom} (stage_common/{a.stcom}.c).\n"
    reg += "".join(f"{n} = 0x{x:08X};{r}\n" for n, x, r in new_syms)
    reg_path.write_text(reg, encoding="utf-8", newline="\n")

    # yaml.
    lines = yaml.split("\n")
    ind = host[4]
    code_lines = [f"{ind}- {{start: 0x{a.at:X}, type: c, name: {a.stcom}, dir: ../stage_common}}"
                  "  # shared stage code"]
    if after_name:
        code_lines.append(f"{ind}- [0x{a.at + size:X}, c, {after_name}]")
    if before:
        lines[host[0]:host[0] + 1] = [lines[host[0]]] + code_lines
    else:
        lines[host[0]:host[0] + 1] = code_lines
    host_rod_done = False
    if rr:
        rline = f"{ind}- {{start: 0x{a.rodata_at:X}, type: .rodata, name: {a.stcom}, dir: ../stage_common}}"
        prev = [s for s in segs if s[2] in (".rodata", "rodata") and s[1] <= a.rodata_at][-1]
        if prev[1] == a.rodata_at and prev[3] == host[3] and not before:
            lines[prev[0]] = rline
            host_rod_done = not after
        else:
            lines.insert(prev[0] + 1, rline)
    if host_rod and not host_rod_done:
        i = next(k for k, l in enumerate(lines) if SEG_RE.match(l) and f", {host[3]}]" in l
                 and ".rodata" in l)
        if before and after:
            split = host_rod[1] + a.host_rodata_split
            lines.insert(i + 1, f"{ind}- [0x{split:X}, .rodata, {after_name}]")
        elif not before and after:
            lines[i] = lines[i].replace(host[3], after_name)
        elif not after:
            pass
    yaml_path.write_text("\n".join(lines), encoding="utf-8", newline="\n")

    # Sources.
    if before:
        host_c.write_text(pre + "\n".join(before).rstrip() + "\n", encoding="utf-8",
                          newline="\n")
    else:
        host_c.unlink()
    if after_name:
        (src_dir / f"{after_name}.c").write_text(
            pre + "\n".join(after).rstrip() + "\n", encoding="utf-8", newline="\n")

    # Headers: drop the moved names' declarations; other uses get the new names.
    word = re.compile(r"\b(?:func|D)_[0-9A-F]{8}\b")
    for h in sorted((ROOT / f"include/{a.stage}").glob("*.h")):
        text = h.read_text(encoding="utf-8")
        keep = [l for l in text.split("\n")
                if not (set(word.findall(l)) & renames.keys() and re.search(r";\s*(//.*)?$", l)
                        and not l.startswith(" "))]
        new = "\n".join(keep)
        new = word.sub(lambda m: renames.get(m.group(0), m.group(0)), new)
        if new != text:
            h.write_text(new, encoding="utf-8", newline="\n")
    for c in sorted(src_dir.glob("*.c")):
        text = c.read_text(encoding="utf-8")
        new = word.sub(lambda m: renames.get(m.group(0), m.group(0)), text)
        if new != text:
            c.write_text(new, encoding="utf-8", newline="\n")
            print(f"  renamed uses in {c.relative_to(ROOT)}")
    print("applied")


if __name__ == "__main__":
    main()
