#!/usr/bin/env bash
# Generates a Doxygen-friendly mainpage from Readme.md: strips the raw
# <img> logo tag from the H1 (Doxygen's markdown parser doesn't render
# inline HTML inside heading/tree titles, so it was leaking as literal
# text into the sidebar). GitHub's own README rendering is untouched;
# this only affects the generated docs site.
set -euo pipefail
cd "$(dirname "$0")/.."
sed -E '1s/<img[^>]*\/>\s*//' Readme.md > docs/mainpage.md
