#!/usr/bin/env python3
"""Builds docs/mainpage.md from Readme.md for the Doxygen/Pages build.

Two problems with feeding Readme.md straight to Doxygen:
  1. Its H1 mixes a raw <img> logo tag with the title text, which Doxygen's
     markdown parser doesn't render inside heading/tree-view titles.
  2. Doxygen's own "native" Mermaid support shells out to mermaid-cli at
     build time and is inconsistent across doxygen versions/runners.

So this script strips the logo tag from the H1, and pre-renders every
```mermaid fenced block to a standalone SVG (via @mermaid-js/mermaid-cli
through npx), replacing the fence with a plain markdown image reference.
Doxygen then just displays a picture -- nothing mermaid-specific required
at doc-generation time.
"""
import re
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
README = ROOT / "Readme.md"
DOCS = ROOT / "docs"
GENERATED = DOCS / "generated"
OUT = DOCS / "mainpage.md"
PUPPETEER_CFG = DOCS / "puppeteer-config.json"

MERMAID_RE = re.compile(r"```mermaid\n(.*?)```", re.DOTALL)

def main():
    text = README.read_text()

    # Strip the raw <img> logo tag from the H1 line only.
    text = re.sub(r'^# <img[^>]*/>\s*', '# ', text, count=1, flags=re.MULTILINE)

    GENERATED.mkdir(parents=True, exist_ok=True)

    def render(match: "re.Match[str]") -> str:
        render.count += 1
        idx = render.count
        mmd = GENERATED / f"diagram-{idx}.mmd"
        svg = GENERATED / f"diagram-{idx}.svg"
        mmd.write_text(match.group(1))
        subprocess.run(
            [
                "npx", "-y", "-p", "@mermaid-js/mermaid-cli", "mmdc",
                "-i", str(mmd), "-o", str(svg),
                "-b", "transparent",
                "-p", str(PUPPETEER_CFG),
            ],
            check=True,
        )
        return f"![diagram {idx}](generated/diagram-{idx}.svg)"

    render.count = 0
    text = MERMAID_RE.sub(render, text)

    OUT.write_text(text)
    print(f"wrote {OUT} with {render.count} rendered diagram(s)")

if __name__ == "__main__":
    sys.exit(main())
