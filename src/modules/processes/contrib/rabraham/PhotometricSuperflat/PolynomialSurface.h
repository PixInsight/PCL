#ifndef __PolynomialSurface_h
#define __PolynomialSurface_h

#include <pcl/Math.h>
#include <pcl/File.h>
#include <pcl/ByteArray.h>
#include <pcl/Matrix.h>
#include <pcl/Console.h>
#include <pcl/ExternalProcess.h>
#include <pcl/Matrix.h>
#include <pcl/Algebra.h>

#define MAX_POLYNOMIAL_TERMS 15

namespace pcl {

    // ----------------------------------------------------------------------------

    class Star {
        
    public:

        String date_obs;
        String name;
        String filter;
        double catra;
        double catdec;
        double imgra;
        double imgdec;
        double imgcx;
        double imgcy;
        double mag;
        double bkground;
        double bgstddev;
        double bgrjct;
        double psf_a;
        double psf_sigmax;
        double psf_sigmay;
        double psf_theta;
        double psf_mad;
        double flux8;
        double snr8;
        String flag;
        double zeropoint;
        double relativeFlux;

        Star(String line) {
            StringList tokens;
            line.Break(tokens, ';'); // Break into substrings

            date_obs = tokens[0];
            name = tokens[1];
            filter = tokens[2];
            catra = tokens[3].ToDouble();
            catdec = tokens[4].ToDouble();
            imgra = tokens[5].ToDouble();
            imgdec = tokens[6].ToDouble();
            imgcx = tokens[7].ToDouble();
            imgcy = tokens[8].ToDouble();
            mag = tokens[9].ToDouble();
            bkground = tokens[10].ToDouble();
            bgstddev = tokens[11].ToDouble();
            bgrjct = tokens[12].ToDouble();
            psf_a = tokens[13].ToDouble();
            psf_sigmax = tokens[14].ToDouble();
            psf_sigmay = tokens[15].ToDouble();
            psf_theta = tokens[16].ToDouble();
            psf_mad = tokens[17].ToDouble();
            flux8 = tokens[18].ToDouble();
            snr8 = tokens[19].ToDouble();
            flag = tokens[20];

            // Derived quantities
            zeropoint = mag + 2.5 * log10(flux8);
            relativeFlux = 1.0; // Dummy; this gets filled in later
        }

        void PrintStar() {
            Console().Write(name);
            Console().Write(String().Format("  %8g   %8g   %8g   %12g   %8g   ", imgcx, imgcy, mag, flux8, zeropoint));
            Console().WriteLn(flag);

        };


    }; // Star


    // ----------------------------------------------------------------------------

    class PolynomialSurface {
    public:

        PolynomialSurface(const String& fileName = String(), int nx = 0, int ny = 0) {
            // Initialize everything
            this->nx = nx;
            this->ny = ny;
            for (int i = 0; i < MAX_POLYNOMIAL_TERMS; i++)
                coeffs[i] = 0.0;
            signalToNoiseLevelCut = 0.0;
            badStarsFilteredOut = false;
            relativeFluxesComputed = false;
            modelComputed = false;

            if (!fileName.IsEmpty())
                ReadStarCatalog(fileName);
        }

        virtual ~PolynomialSurface() {
        }

        void PlotXYKeyedToRelativeFlux(bool useCorrection) {
            File dataFile;
            File gnuFile;
            File svgFile;

            String tmpDir = File().SystemTempDirectory();
            String dataFilepath = File().UniqueFileName(tmpDir,8,"",".dat");
            String gnuFilepath = File().UniqueFileName(tmpDir,8,"",".gnu");
            String svgFilepath = File().UniqueFileName(tmpDir,8,"",".svg");
            
            // Make sure we're ready to plot the data.
            if (!relativeFluxesComputed)
                ConvertZeroPointToRelativeFlux();

            // Generate the data table, but clip it.
            dataFile.CreateForWriting(dataFilepath);
            for (Array<Star>::iterator s = goodStars.Begin(); s != goodStars.End(); ++s) {
                double correction = 1.0;
                double clippedRelativeFlux;
                if (useCorrection)
                    correction = ValueAt(s->imgcx, s->imgcy);
                clippedRelativeFlux = (s->relativeFlux) / correction;
                if (clippedRelativeFlux > 1.2)
                    clippedRelativeFlux = 1.2;
                if (clippedRelativeFlux < 0.8)
                    clippedRelativeFlux = 0.8;
                dataFile.OutTextLn(IsoString().Format("%8g   %8g    %8g    %g", s->imgcx, s->imgcy, clippedRelativeFlux,  correction));
            }
            dataFile.Flush();
            dataFile.Close();

            // Generate the plot commands
            gnuFile.CreateForWriting(gnuFilepath);
            gnuFile.OutTextLn("set terminal svg size 800,600 enhanced font 'helvetica,12'");
            gnuFile.OutTextLn("set object 1 rectangle from screen 0,0 to screen 1,1 fillcolor rgb'#ffffff' behind");
            gnuFile.OutTextLn("unset key");
            gnuFile.OutTextLn(IsoString("set output '" + svgFilepath + "'"));
            if (!useCorrection)
                gnuFile.OutTextLn("set title 'Relative fluxes of calibration stars before superflat correction' font 'helvetica,16' ");
            else
                gnuFile.OutTextLn("set title 'Relative fluxes of calibration stars after superflat correction' font 'helvetica,16' ");
            gnuFile.OutTextLn(IsoString().Format("set xrange [1:%d]", nx));
            gnuFile.OutTextLn(IsoString().Format("set yrange [1:%d]", ny));
            gnuFile.OutTextLn("set palette rgb 33,13,10");
            gnuFile.OutTextLn(IsoString("plot '" + dataFilepath + "' using 1:2:3 lt pal pt 7 ps 0.5"));
            gnuFile.Flush();
            gnuFile.Close();

            
            // I need to determine the location of the PixInsight executable so I can 
            // find the location of the gnuplot executable. I figured this might be in an
            // environment variable but I can't seem to get at any environment variables. For now
            // I will just hard-code the path to the executable
            //
            //StringList env = ExternalProcess().Environment();
            //Console().WriteLn("Known environment variables:");
            //for (StringList::const_iterator row = env.Begin(); row != env.End(); ++row) {
            //    Console().WriteLn(*row);
            //}
            
            // Create the plot
            ExternalProcess().ExecuteProgram("/Applications/PixInsight.app/Contents/MacOS/gnuplot " + gnuFilepath);

            // Display the plot window
            ImageWindow().Open(svgFilepath)[0].Show();
            
            //Clean up
            File().Remove(gnuFilepath);
            File().Remove(dataFilepath);
            File().Remove(svgFilepath);
        }

        void PrintCatalog() {
            Console().WriteLn("         Name                  X         Y          Mag         Flux           ZP         FLAG");
            Console().WriteLn("------------------------    --------  --------    --------   -------------   ---------  ----------");
            for (Array<Star>::iterator s = allStars.Begin(); s != allStars.End(); ++s)
                s->PrintStar();
        }

        void PrintCatalogSummary() {
            Console().WriteLn(String().Format("Catalog contains %d stars", allStars.Length()));
        }

        void ShowBestFitModelImage() {
            int numberOfChannels = 1;
            int bitsPerSample = 32;
            bool floatSample = true;
            bool isColor = false;
            bool initialProcessing = true;
            String id = "Superflat";


            Console().WriteLn(String().Format("Creating array with dimensions %d x %d", this->nx, this->ny));
            Console().Flush();
            ImageWindow w(this->nx, this->ny, numberOfChannels, bitsPerSample, floatSample, isColor, initialProcessing, id);

            try {
                // Extract the image data from the newly-created window
                ImageVariant v = w.MainView().Image();
                GenericImage<pcl::FloatPixelTraits>& image = static_cast<GenericImage<pcl::FloatPixelTraits>&> (*v);

                Console().WriteLn(String().Format("Writing to %d x %d image", image.Width(), image.Height()));

                // Write values to the image.
                for (int i = 0; i < image.Width(); i++) {
                    for (int j = 0; j < image.Height(); j++) {
                        image.Pixel(i, j, 0) = (float) ValueAt((double) i, (double) j);
                    }
                }
                Console().WriteLn("Finished writing Array to memory");
                Console().Flush();

                // The image should be normalized so its maximum value is unity
                image /= image.MaximumSampleValue();
                w.Show();
                w.ZoomToFit(false);
            }            catch (...) {
                w.Close();
                throw;
            }

        }

        String ComputeBestFitModel(int degree) {

            // Reset the model
            Console().WriteLn(String().Format("Computing PolynomialSurface of degree %d.", degree));
            this->degree = degree;
            for (int i = 0; i < MAX_POLYNOMIAL_TERMS; i++)
                this->coeffs[i] = 0.0;

            // I will use Stetson's notation. See http://ned.ipac.caltech.edu/level5/Stetson/Stetson1_2.html
            //
            // Define a model with the following dependent variables. These are the t_i in Stetson's notation:

            // degree = 0: t = {1}
            // degree = 1: t = {1, x, y}
            // degree = 2: t = {1, x, y, x y, x^2, y^2}
            // degree = 3: t = {1, x, y, x y, x^2, y^2, x^2 y, x y^2, x^3, y^3}
            // degree = 4: t = {1, x, y, x y, x^2, y^2, x^2 y, x y^2, x^3, y^3, x y^3, x^2 y^2, x^3 y, x^4, y^4}
            //
            // For example, if degree = 1, then the linear system has three unknown coefficients: A, B, C, and the
            // equation is:
            //
            // z = A*(1) + B*(x) +C*(y)

            // Number of unknowns
            switch (degree) {
                case 0:
                    nterms = 1;
                    break;
                case 1:
                    nterms = 3;
                    break;
                case 2:
                    nterms = 6;
                    break;
                case 3:
                    nterms = 10;
                    break;
                case 4:
                    nterms = 15;
                    break;
            }

            // Describe the terms in the polynomial, so we can print them out nicely.
            int maxterms = 15;
            StringList poly(maxterms);
            poly[0] = "1";
            poly[1] = "x";
            poly[2] = "y";
            poly[3] = "x*y";
            poly[4] = "x**2";
            poly[5] = "y**2";
            poly[6] = "x**2 * y";
            poly[7] = "x * y**2";
            poly[8] = "x**3";
            poly[9] = "y**3";
            poly[10] = "x * y**3";
            poly[11] = "x**2 * y**2";
            poly[12] = "x**3 * y";
            poly[13] = "x**4";
            poly[14] = "y**4";

            // Do some basic pre-processing of the data
            if (!badStarsFilteredOut)
                FilterStars();
            if (!relativeFluxesComputed)
                ConvertZeroPointToRelativeFlux();

            // Organize the data
            Console().WriteLn(String().Format("Analyzing catalog with %d stars", goodStars.Length()));
            Vector x(0.0, goodStars.Length());
            Vector y(0.0, goodStars.Length());
            Vector z(0.0, goodStars.Length());
            Vector w(0.0, goodStars.Length());
            Matrix t(0.0, goodStars.Length(), nterms);
            for (unsigned int i = 0; i < goodStars.Length(); i++) {

                x[i] = goodStars[i].imgcx;
                y[i] = goodStars[i].imgcy;
                z[i] = goodStars[i].relativeFlux;
                w[i] = 1.0;

                switch (degree) {

                        // degree = 0: constant
                        // {1}
                    case 0:
                        t[i][0] = 1.0;
                        break;

                        // degree = 1: plane
                        // {1, x, y}
                    case 1:
                        t[i][0] = 1.0;
                        t[i][1] = x[i];
                        t[i][2] = y[i];
                        break;

                        // degree = 2: quadratic
                        // {1, x, y, x y, x^2, y^2}
                    case 2:
                        t[i][0] = 1.0;
                        t[i][1] = x[i];
                        t[i][2] = y[i];
                        t[i][3] = x[i] * y[i];
                        t[i][4] = x[i] * x[i];
                        t[i][5] = y[i] * y[i];
                        break;

                        // degree = 3: cubic
                        // {1, x, y, x y, x^2, y^2, x^2 y, x y^2, x^3, y^3}
                    case 3:
                        t[i][0] = 1.0;
                        t[i][1] = x[i];
                        t[i][2] = y[i];
                        t[i][3] = x[i] * y[i];
                        t[i][4] = x[i] * x[i];
                        t[i][5] = y[i] * y[i];
                        t[i][6] = x[i] * x[i] * y[i];
                        t[i][7] = x[i] * y[i] * y[i];
                        t[i][8] = x[i] * x[i] * x[i];
                        t[i][9] = y[i] * y[i] * y[i];
                        break;


                        // degree = 4: quartic
                        // {1, x, y, x y, x^2, y^2, x^2 y, x y^2, x^3, y^3, x y^3, x^2 y^2, x^3 y, x^4, y^4}
                    case 4:
                        t[i][0] = 1.0;
                        t[i][1] = x[i];
                        t[i][2] = y[i];
                        t[i][3] = x[i] * y[i];
                        t[i][4] = x[i] * x[i];
                        t[i][5] = y[i] * y[i];
                        t[i][6] = x[i] * x[i] * y[i];
                        t[i][7] = x[i] * y[i] * y[i];
                        t[i][8] = x[i] * x[i] * x[i];
                        t[i][9] = y[i] * y[i] * y[i];
                        t[i][10] = x[i] * y[i] * y[i] * y[i];
                        t[i][11] = x[i] * x[i] * y[i] * y[i];
                        t[i][12] = x[i] * x[i] * x[i] * y[i];
                        t[i][13] = x[i] * x[i] * x[i] * x[i];
                        t[i][14] = y[i] * y[i] * y[i] * y[i];
                        break;

                }
            }

            // Construct the matrix and vector at the heart of the linear system. This is Step 2 in Stetson's paper.
            Matrix M(0.0, nterms, nterms);
            for (int i = 0; i < nterms; i++) {
                for (int j = 0; j < nterms; j++) {
                    for (unsigned int k = 0; k < goodStars.Length(); k++) {
                        M[i][j] += t[k][i] * t[k][j];
                    }
                }
            }

            Matrix V(0.0, nterms, 1);
            for (int i = 0; i < nterms; i++) {
                for (unsigned int j = 0; j < goodStars.Length(); j++) {
                    V[i][0] += t[j][i] * z[j];
                }
            }

            // Display the linear system
            Console().WriteLn(String().Format("Solving the following linear system of degree %d:", degree));
            Console().WriteLn("");
            Console().WriteLn("M*A = V");
            Console().WriteLn("");
            Console().WriteLn("M = ");
            for (int i = 0; i < nterms; i++) {
                for (int j = 0; j < nterms; j++) {
                    Console().Write(String().Format("%12g ", M[i][j]));
                }
                Console().WriteLn("");
            }
            Console().WriteLn("");

            Console().WriteLn("V = ");
            for (int i = 0; i < nterms; i++) {
                Console().Write(String().Format("%12g ", V[i][0]));
            }
            Console().WriteLn("");


            // Solve M A = V, where A is a vector with the best-fit parameters that we want.
            Matrix A = M.Inverse() * V;

            // Format the result nicely and output it to the console
            String result = "";
            for (int i = 0; i < nterms; i++) {
                if (i == 0) {
                    result += String().Format("%12g ", A[0][i]);
                } else {
                    result += String().Format("(%12g * ", A[0][i]);
                    result += poly[i];
                    result += ")";
                }

                if (i < (nterms - 1))
                    result += " + ";
            }
            Console().WriteLn("");
            Console().WriteLn("Equation of best-fit surface z = f(x,y):");
            Console().WriteLn("");
            Console().Write("z = ");
            Console().WriteLn(result);
            Console().WriteLn("");

            this->equation = result;

            // Store the result
            for (int i = 0; i<nterms; i++)
                this->coeffs[i] = (double) A[0][i];

            modelComputed = true;
            return (this->equation);

        }




    private:

        File file;
        Array<Star> allStars;
        Array<Star> goodStars;
        double coeffs[MAX_POLYNOMIAL_TERMS]; // Coefficients of the best-fit polynomial
        int degree; // Degree of the best-fit polynomial (0=constant, 1=plane, etc)
        int nterms; // Number of terms in the best-fit polynomial
        int nx; // X dimension of input image
        int ny; // Y dimension of input image
        String equation; // Pretty-printed string describing the best fit polynomial
        double signalToNoiseLevelCut; // Minimum signal-to-noise of stars to include in the computation
        bool badStarsFilteredOut; // Have bad stars already been removed?
        bool relativeFluxesComputed; // Have relative fluxes already been computed? 
        bool modelComputed; // Surface has been defined

        void ReadStarCatalog(const String& fileName) {
            IsoStringList lines;
            int nheader = 5;

            Console().Write("Attempting to read ");
            Console().WriteLn(fileName);
            lines = File::ReadLines(fileName, ReadTextOption::RemoveEmptyLines | ReadTextOption::TrimTrailingSpaces | ReadTextOption::TrimLeadingSpaces);
            Console().WriteLn(String().Format("Read %u lines.", lines.Length()));

            Console().WriteLn("Loading stars into the PolynomialSurface object.");
            for (IsoStringList::const_iterator row = lines.Begin() + nheader; row != lines.End(); ++row) {
                Star *s = new Star(*row);
                AddStar(*s);
            }
            goodStars = Array<Star>(allStars);
        }

        void AddStar(Star& s) {
            allStars.Append(s);
        }

        void ConvertZeroPointToRelativeFlux() {
            if (!badStarsFilteredOut)
                FilterStars();
            Vector relative_flux(0.0, goodStars.Length());
            for (unsigned int i = 0; i < goodStars.Length(); i++)
                relative_flux[i] = pow(10.0, 0.4 * goodStars[i].zeropoint);

            double med;
            Vector const_relative_flux = Vector(relative_flux);
            med = const_relative_flux.Median();  // Need to call it on a const Vector or it scrambles
            relative_flux /= med;
            for (unsigned int i = 0; i < goodStars.Length(); i++)
                goodStars[i].relativeFlux = relative_flux[i];
            relativeFluxesComputed = true;
            Console().WriteLn(String().Format("Computed relative fluxes for %d stars.", goodStars.Length()));
        }

        void FilterStars() {
            goodStars.Clear();
            for (unsigned int i = 0; i < allStars.Length(); i++) {
                if (allStars[i].flag == "0000") {
                    goodStars.Append(allStars[i]);
                }
            }
            badStarsFilteredOut = true;
            Console().WriteLn(String().Format("Filtered catalog contains %d stars.", goodStars.Length()));
        }

        double ValueAt(double x, double y) {
            double t[MAX_POLYNOMIAL_TERMS];
            double val;

            t[0] = 1.0;
            t[1] = x;
            t[2] = y;
            t[3] = x * y;
            t[4] = x * x;
            t[5] = y * y;
            t[6] = x * x * y;
            t[7] = x * y * y;
            t[8] = x * x * x;
            t[9] = y * y * y;
            t[10] = x * y * y * y;
            t[11] = x * x * y * y;
            t[12] = x * x * x * y;
            t[13] = x * x * x * x;
            t[14] = y * y * y * y;
            
            val = 0.0;
            for (int i = 0; i < nterms; i++) {
                val += this->coeffs[i] * t[i];
            }

            return (val);

        }

    }; // PolynomialSurface

    // ----------------------------------------------------------------------------

} // pcl

#endif   // __PolynomialSurface_h

