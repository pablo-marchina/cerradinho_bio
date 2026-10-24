"""Conta as palavras de uma seção da documentacao.md, para conferir limite de palavras quando a avaliação tiver um.

Uso: python scripts/count_words.py ARQUIVO.md --section 3.1.4 [--ref BRANCH] [--limit N]

Com --ref, lê o ARQUIVO (caminho relativo à raiz do repositório, ex.: document/documentacao.md)
na branch remota indicada, via `git show origin/BRANCH:ARQUIVO`; rode `git fetch` antes.

Regra: conta como palavra todo token com letra ou número ("50%", "R01" e "[6]" contam),
ignorando URLs de links e imagens, pipes, setas e traços soltos. Mostra o total com e sem
título, legenda, fonte e texto alternativo, porque a regra oficial de contagem costuma não estar escrita.
"""
import argparse
import os
import re
import subprocess
import sys

LINK_TARGET = re.compile(r"\]\([^)]*\)")
WORD = re.compile(r"\w", re.UNICODE)


def count(text):
    text = LINK_TARGET.sub("]", text)
    return sum(1 for tok in text.split() if WORD.search(tok))


def section_lines(lines, number):
    start = level = None
    for i, line in enumerate(lines):
        m = re.match(r"^(#+)\s+(.*)", line)
        if not m:
            continue
        if start is None and re.match(rf"{re.escape(number)}\.?(\s|$)", m.group(2).strip()):
            start, level = i, len(m.group(1))
        elif start is not None and len(m.group(1)) <= level:
            return lines[start:i]
    if start is None:
        sys.exit(f"Seção {number} não encontrada.")
    return lines[start:]


def kind(line):
    s = line.strip()
    if s.startswith("#"):
        return "título"
    if s.startswith("!["):
        return "texto alternativo"
    if re.match(r"^\*?(Figura|Fonte|Tabela)\b", s.lstrip("*_ ")):
        return "legenda e fonte"
    if s.startswith("|"):
        return "tabela"
    return "texto"


def _repo_root():
    return subprocess.run(["git", "rev-parse", "--show-toplevel"], capture_output=True, text=True,
                          encoding="utf-8", check=True).stdout.strip()


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("arquivo")
    ap.add_argument("--section", required=True, help="número da seção, ex.: 3.1.4")
    ap.add_argument("--ref", help="branch remota de onde ler o arquivo (ex.: a source branch do MR)")
    ap.add_argument("--limit", type=int, help="limite de palavras da avaliação, se houver")
    args = ap.parse_args()
    sys.stdout.reconfigure(encoding="utf-8")

    if args.ref:
        path = os.path.relpath(os.path.abspath(args.arquivo), _repo_root()).replace(os.sep, "/")
        text = subprocess.run(["git", "show", f"origin/{args.ref}:{path}"], cwd=_repo_root(),
                              capture_output=True, check=True).stdout.decode("utf-8")
    else:
        with open(args.arquivo, encoding="utf-8") as fh:
            text = fh.read()
    lines = section_lines(text.splitlines(), args.section)

    parts = {}
    for line in lines:
        if line.strip():
            parts[kind(line)] = parts.get(kind(line), 0) + count(line)
    total = sum(parts.values())
    extras = sum(parts.get(k, 0) for k in ("título", "texto alternativo", "legenda e fonte"))

    for name, value in parts.items():
        print(f"{name:<24} {value:>4}")
    print(f"{'total':<24} {total:>4}")
    print(f"{'sem título/legenda/alt':<24} {total - extras:>4}")
    if args.limit:
        status = "dentro" if total <= args.limit else (
            "só sem título/legenda/alt" if total - extras <= args.limit else "fora")
        print(f"limite {args.limit}: {status}")


if __name__ == "__main__":
    main()
