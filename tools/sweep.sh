#!/usr/bin/env bash
# Replace old symbol names with new ones everywhere rename.py doesn't reach: the overlay sources
# (src/r3_*, src/stage_common/), port/, and any leftovers in src/, include/ and notes/.
# Run it after tools/rename.py, then re-split and build the overlays.
#
# PAIRS is a text file of "OLD NEW" lines (extra columns, blank lines and # comments are ignored),
# the same pairs given to rename.py. Matches whole words only.
#
# Prints how many files it changed and how many still hold an old name, and exits 1 if any do,
# so a caller (or an agent running it) can't report success without the change.
#
# Usage: tools/sweep.sh PAIRS [-n]     (-n: dry run, list the files without changing them)
set -euo pipefail

pairs="${1:?usage: tools/sweep.sh PAIRS [-n]}"
dry="${2:-}"
dirs=(src include notes)
[ -d port ] && dirs+=(port)

scratch=logs/scratch
mkdir -p "$scratch"
old="$scratch/sweep_old.txt"
script="$scratch/sweep.sed"
: > "$old"
: > "$script"

# Written with printf '%s', never as a format string: a literal \b in a format (or in awk) becomes
# a backspace character, and the patterns then match nothing.
b='\b'
count=0
while read -r o n _; do
    case "$o" in ''|'#'*) continue ;; esac
    if [ -z "${n:-}" ] || [[ "$n" == '#'* ]]; then
        echo "sweep: no new name for $o" >&2
        exit 2
    fi
    printf '%s\n' "$b$o$b" >> "$old"
    printf '%s\n' "s/$b$o$b/$n/g" >> "$script"
    count=$((count + 1))
done < "$pairs"

if [ "$count" -eq 0 ]; then
    echo "sweep: no pairs in $pairs" >&2
    exit 2
fi

files=$(grep -rlE -f "$old" "${dirs[@]}" || true)
nfiles=$(printf '%s' "$files" | grep -c . || true)

if [ "$dry" = "-n" ]; then
    printf '%s\n' "$files" | grep . || true
    echo "sweep: $count pairs, $nfiles files would change (dry run)"
    exit 0
fi

if [ "$nfiles" -gt 0 ]; then
    printf '%s\n' "$files" | xargs sed -i -E -f "$script"
fi

left=$(grep -rlE -f "$old" "${dirs[@]}" | grep -c . || true)
echo "sweep: $count pairs, $nfiles files changed, $left files still have an old name"
[ "$left" -eq 0 ]
