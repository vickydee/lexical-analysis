# How it Works
A lexical analyzer scans an input char-by-char and accordingly group them into tokens based on grammar rules.

In this case, we have three NEW tokens:
* REALNUM (decimal with a fractional part)
* BASE08NUM (octal-style form ending in x08)
* BASE16NUM (hex-style form ending in x16, allows A–F)

As well as the existing:
* NUM (use the existing ScanNumber())

# How to Install
git clone https://github.com/vickydee/cse340project1.git
