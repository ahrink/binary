skeleton and correlation to tarballs – the AHRtID README

(the praevisio got better)

In this version, we demonstrate the usage of a tarball specifically automated for developers—where developers are users working on larger projects. What we are presenting in this study is AHRtID, a binary, and how it was created—open source.

Due to the fact that this project also involves a form of protection for both man and machine, this binary will change its form when issued in real time to users. As a result, every approved developer will receive a unique binary that takes part in the compilation of a tarball for submission to GVF (Global Value Foundation).

The GVF review process is simple: a replacement of all files found in <skeleton>/bin with
(a) the unique developer binary in the GVF inventory, and
(b) the reviewed open sources that are attached to the package.

The (a) and (b) process is an assurance, in that a tarball is cleaned of any malicious, buggy, or dysfunctional artifacts, which in turn determines whether the package is usable or unusable.

General skeleton structure understanding — describing a tarball as a package

The files in the root of the skeleton are reserved only for package manipulation and code-hardening methods. Per se, this creates an environment. It ensures that a package can be deployed via SSH, used on a USB data disk, or run on a local persistent OS versus a Live OS.

A tar.gz archive is also widely used on the Internet to download sources and indirectly involves data lakes.

<skeleton>/bin — reserved for compiled binaries taking part in the project

<skeleton>/dev — reserved for current developments, specific to a project

<skeleton>/docs — AI- and human-parseable files (txt, html, pdf)

<skeleton>/pdf — PDF only

<skeleton>/pub — reserved for LaTeX

<skeleton>/xC — C language sources that took part in compilation of the binary

<skeleton>/xL — implementations using other programming-language specifics

<skeleton>/yNdx — “Last pages of a Book,” aka SEO or old-fashioned index

<skeleton>/zPS — other remarks and keynotes (REM, etc.)

PS: Research? If it is not there in the package, it means it is not important—yet.
Note: The tar manifest 004_state.txt is intentionally left blank, as the scope of this study is focused on AHRtID.
