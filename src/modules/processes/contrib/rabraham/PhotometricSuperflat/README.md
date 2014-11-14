PhotometricSuperflat
====================

PixInsight module to create a photometric superflat

This module reads a table produced by the AperturePhotometry script, fits a
polynomial surface (of degree up to 4) to the data and produces a photometric
superflat frame as well as a couple of plots showing the relative flux errors
in the calibration stars with and without the application of the flat. Here is
a screenshot showing it in use:

https://dl.dropboxusercontent.com/u/110230258/SuperflatScreenshot.png

The easiest way to test it out is with simulated data. To do this, generate a
fake image using the CatalogStarGenerator script, then modify this image with
some function using the PixelMath module. (It's obviously important that you
modify the image using some sort of multiplicative process and not an additive
process). Run the AperturePhotometry script on the simulated image to generate
a catalog. Then fire up the PhotometricSuperflat process and click on the
button to select the catalog (whose filename will end with _WCS.csv). The
drag the process icon onto the image you have just photometered to generate the
Superflat and diagnostic plots.

Notes on modifying the source code
----------------------------------

I find the trickiest part of creating PixInsight modules is handling
parameters. When you add a new parameter you need to add appropriate code in
these four files:

- PhotometricSuperflatParameters

- PhotometricSuperflatInterface

- PhotometricSuperflatInstance

- PhotometricSuperflatProcess  [Note: instantiate the process parameters in the
  constructor or you will get a segfault]
