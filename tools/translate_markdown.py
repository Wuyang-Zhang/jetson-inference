#!/usr/bin/env python3
import argparse
import html
import json
import os
import re
import subprocess
import time
import urllib.parse
import urllib.request
from pathlib import Path


PLACEHOLDER_RE = re.compile(r"__PH_(\d+)__")
URL_PLACEHOLDER_RE = re.compile(r"__URLPH_(\d+)__")
TRANSLATION_CACHE: dict[str, str] = {}
GLOSSARY_REPLACEMENTS = {
    "对象检测": "目标检测",
    "图片分类": "图像分类",
    "教程步骤": "教程步骤",
    "实时相机": "实时摄像头",
}
ZERO_WIDTH_CHARS = dict.fromkeys(map(ord, "\u200b\u200c\u200d\ufeff"), None)


def fetch_text(url: str, headers: dict[str, str] | None = None) -> str:
    req = urllib.request.Request(url, headers=headers or {})
    with urllib.request.urlopen(req, timeout=30) as resp:
        return resp.read().decode("utf-8", "ignore")


def google_mobile_translate(text: str) -> str:
    query = urllib.parse.quote(text)
    url = f"https://translate.google.com/m?sl=en&tl=zh-CN&q={query}"
    body = fetch_text(url, headers={"User-Agent": "Mozilla/5.0"})
    marker = 'class="result-container">'
    if marker not in body:
        raise RuntimeError("google mobile response format changed")
    translated = body.split(marker, 1)[1].split("<", 1)[0]
    return html.unescape(translated)


def mymemory_translate(text: str) -> str:
    query = urllib.parse.quote(text)
    url = f"https://api.mymemory.translated.net/get?q={query}&langpair=en|zh-CN"
    body = fetch_text(url, headers={"User-Agent": "Mozilla/5.0"})
    payload = json.loads(body)
    return payload["responseData"]["translatedText"]


def translate_text(text: str) -> str:
    if text in TRANSLATION_CACHE:
        return TRANSLATION_CACHE[text]

    errors = []
    for func in (google_mobile_translate, mymemory_translate):
        try:
            translated = func(text)
            TRANSLATION_CACHE[text] = translated
            return translated
        except Exception as exc:  # pragma: no cover - network-dependent fallback
            errors.append(f"{func.__name__}: {exc}")
            time.sleep(1)
    raise RuntimeError("; ".join(errors))


def protect_inline(text: str) -> tuple[str, list[str]]:
    placeholders: list[str] = []
    def protect_link_urls(match: re.Match[str]) -> str:
        bang = match.group(1)
        label = match.group(2)
        url = match.group(3)
        placeholders.append(url)
        return f"{bang}[{label}](__URLPH_{len(placeholders)-1}__)"

    text = re.sub(r"(!?)\[([^\]]*)\]\(([^)]+)\)", protect_link_urls, text)

    patterns = [
        r"```[\s\S]*?```",
        r"`[^`\n]+`",
        r"<img\b[^>]*>",
        r"https?://[^\s)]+",
    ]
    combined = re.compile("|".join(f"({p})" for p in patterns), re.MULTILINE)

    def repl(match: re.Match[str]) -> str:
        placeholders.append(match.group(0))
        return f"__PH_{len(placeholders)-1}__"

    text = combined.sub(repl, text)
    return text, placeholders


def restore_inline(text: str, placeholders: list[str]) -> str:
    text = text.translate(ZERO_WIDTH_CHARS)

    def repl(match: re.Match[str]) -> str:
        idx = int(match.group(1))
        return placeholders[idx]

    text = PLACEHOLDER_RE.sub(repl, text)
    text = URL_PLACEHOLDER_RE.sub(repl, text)
    return text


def split_blocks(text: str) -> list[str]:
    parts: list[str] = []
    current: list[str] = []

    def flush() -> None:
        if current:
            parts.append("\n".join(current))
            current.clear()

    for line in text.splitlines():
        if not line.strip():
            flush()
            parts.append("")
            continue
        if re.match(r"^\s{0,3}([-*+]|\d+\.)\s", line) or line.startswith(">"):
            flush()
            parts.append(line)
            continue
        current.append(line)

    flush()
    return parts


def translate_block(block: str) -> str:
    if not block:
        return ""
    if re.fullmatch(r"\s*-{3,}\s*", block):
        return block

    heading = re.match(r"^(#{1,6}\s+)(.+)$", block)
    if heading:
        return heading.group(1) + translate_block(heading.group(2))

    list_item = re.match(r"^(\s{0,3}(?:[-*+]|\d+\.)\s+)(.+)$", block)
    if list_item:
        return list_item.group(1) + translate_block(list_item.group(2))

    quote = re.match(r"^(\s*>+\s*)(.+)$", block)
    if quote:
        return quote.group(1) + translate_block(quote.group(2))

    protected, placeholders = protect_inline(block)
    translated = translate_text(protected)
    translated = restore_inline(translated, placeholders)
    translated = translated.replace("：", ": ")
    for src, dst in GLOSSARY_REPLACEMENTS.items():
        translated = translated.replace(src, dst)
    return translated


def adjust_local_markdown_links(text: str, dst: Path) -> str:
    def convert_url(url: str) -> str | None:
        if "://" in url or url.startswith("#"):
            return None

        anchor = ""
        if "#" in url:
            url, anchor = url.split("#", 1)
            anchor = f"#{anchor}"

        if not url.lower().endswith(".md"):
            return None

        path_obj = Path(url)
        if re.search(r"(?:-|\s)CN$", path_obj.stem, re.IGNORECASE):
            return f"{url}{anchor}"

        target = (dst.parent / url).resolve()
        cn_target = target.with_name(f"{target.stem}-CN{target.suffix}")
        try:
            if cn_target.exists() or target.exists():
                rel = os.path.relpath(cn_target, dst.parent)
                rel = rel.replace("\\", "/")
                return f"{rel}{anchor}"
        except Exception:
            return None
        return None

    def md_repl(match: re.Match[str]) -> str:
        bang = match.group(1)
        label = match.group(2)
        url = match.group(3)
        converted = convert_url(url)
        if converted is None:
            return match.group(0)
        return f"{bang}[{label}]({converted})"

    def href_repl(match: re.Match[str]) -> str:
        prefix = match.group(1)
        url = match.group(2)
        converted = convert_url(url)
        if converted is None:
            return match.group(0)
        return f'{prefix}{converted}"'

    text = re.sub(r"(!?)\[([^\]]*)\]\(([^)]+)\)", md_repl, text)
    text = re.sub(r'(href=")([^"]+)"', href_repl, text)
    return text


def target_path(src: Path) -> Path:
    return src.with_name(f"{src.stem}-CN{src.suffix}")


def iter_markdown_files(root: Path) -> list[Path]:
    files = sorted(root.rglob("*.md"))
    return [
        path for path in files
        if not re.search(r"(?:-|\s)CN$", path.stem, re.IGNORECASE)
    ]


def read_source_text(root: Path, src: Path) -> str:
    rel = src.relative_to(root).as_posix()
    try:
        result = subprocess.run(
            ["git", "show", f"HEAD:{rel}"],
            cwd=root,
            check=True,
            capture_output=True,
            text=True,
            encoding="utf-8",
            errors="ignore",
        )
        return result.stdout
    except Exception:
        return src.read_text(encoding="utf-8", errors="ignore")


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--root", default=".")
    parser.add_argument("--limit", type=int, default=0)
    parser.add_argument("--overwrite", action="store_true")
    parser.add_argument("paths", nargs="*")
    args = parser.parse_args()

    root = Path(args.root).resolve()
    if args.paths:
        files = [root / Path(path) for path in args.paths]
    else:
        files = iter_markdown_files(root)
    if args.limit > 0:
        files = files[:args.limit]

    for src in files:
        dst = target_path(src)
        if dst.exists() and not args.overwrite:
            print(f"skip {dst}")
            continue

        text = read_source_text(root, src.resolve())
        blocks = split_blocks(text)
        translated_blocks = []
        for idx, block in enumerate(blocks, start=1):
            if block == "":
                translated_blocks.append("")
                continue
            print(f"[{src}] block {idx}/{len(blocks)}", flush=True)
            translated_blocks.append(translate_block(block))
            time.sleep(0.8)

        output = "\n\n".join(translated_blocks).replace("\n\n\n", "\n\n")
        output = adjust_local_markdown_links(output, dst)
        dst.write_text(output, encoding="utf-8")
        print(f"wrote {dst}")

    return 0


if __name__ == "__main__":
    raise SystemExit(main())
