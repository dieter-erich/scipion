/***************************************************************************
 *
 * Authors:    Sjors Scheres           scheres@cnb.uam.es
 *
 * Unidad de Bioinformatica del Centro Nacional de Biotecnologia , CSIC
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 * 02111-1307  USA
 *
 *  All comments concerning this program package may be sent to the
 *  e-mail address 'xmipp@cnb.uam.es'
 ***************************************************************************/
#include "mlf_tomo.h"

// Read arguments ==========================================================
void Prog_mlf_tomo_prm::read(int argc, char **argv)
{

    // Read command line
    if (checkParameter(argc, argv, "-more_options"))
    {
        usage();
        extended_usage();
    }
    fn_ref = getParameter(argc, argv, "-ref");
    fn_wed = getParameter(argc, argv, "-sumwedge", "");
    fn_wlist = getParameter(argc, argv, "-wedgelist", "");
    SF.read(getParameter(argc, argv, "-i"));
    nr_img = SF.ImgNo();
    fn_root = getParameter(argc, argv, "-o", "mlf_tomo");
    Niter = textToInteger(getParameter(argc, argv, "-iter", "100"));
    fn_frac = getParameter(argc, argv, "-frac", "");
    fix_fractions = checkParameter(argc, argv, "-fix_fractions");
    fix_sigma_noise = checkParameter(argc, argv, "-fix_sigma_noise");
    fix_sigma_offset = checkParameter(argc, argv, "-fix_sigma_offset");
    verb = textToInteger(getParameter(argc, argv, "-verb", "1"));
    istart = textToInteger(getParameter(argc, argv, "-istart", "1"));
    getThreeDoubleParams(argc, argv, "-rot", rot0, rotF, rot_step, 0, 0, 1);
    getThreeDoubleParams(argc, argv, "-tilt", tilt0, tiltF, tilt_step, 0, 0, 1);
    getThreeDoubleParams(argc, argv, "-psi", psi0, psiF, psi_step, 0, 0, 1);
    sigma_offset = textToFloat(getParameter(argc, argv, "-offset", "3"));
    fn_doc = getParameter(argc, argv, "-doc", "");
    fn_sym = getParameter(argc, argv, "-sym", "");
    search_shift = textToFloat(getParameter(argc, argv, "-search_shift", "0"));
    highres = textToFloat(getParameter(argc, argv, "-highres", "-1"));
    lowres = textToFloat(getParameter(argc, argv, "-lowres", "-1"));
    fn_sig = getParameter(argc, argv, "-noise", "");
    fn_solv = getParameter(argc, argv, "-solvent", "");
    fn_solv2 = getParameter(argc, argv, "-solvent2", "");
    theta = textToFloat(getParameter(argc, argv, "-theta", "0"));
    theta0 = textToFloat(getParameter(argc, argv, "-theta0", "0"));
    theta_step = textToFloat(getParameter(argc, argv, "-theta_step", "0"));
    fn_misalign = getParameter(argc, argv, "-misalign", "");
    fn_mask = getParameter(argc, argv, "-mask", "");
    debug = checkParameter(argc, argv, "-debug");

}

// Show ====================================================================
void Prog_mlf_tomo_prm::show()
{

    if (verb > 0)
    {
        // To screen
        cerr << "--> Maximum-likelihood multi-reference refinement " << endl;
        cerr << "  Tomographic volumes     : " << SF.name() << " (" << SF.ImgNo() << ")" << endl;
        cerr << "  Reference volumes       : " << fn_ref << " (" << SFr.ImgNo() << ")" << endl;
        cerr << "  Output rootname         : " << fn_root << endl;
        cerr << "  Number of iterations    : " << Niter << endl;
        cerr << "  Rot search              : " << rot0 << " " << rotF << " " << rot_step << endl;
        cerr << "  Tilt search             : " << tilt0 << " " << tiltF << " " << tilt_step << endl;
        cerr << "  Psi search (wrt axis)   : " << psi0 << " " << psiF << " " << psi_step << endl;
        if (fn_sym != "")
            cerr << "  Symmetry file           : " << fn_sym << endl;
        if (fn_mask != "")
            cerr << "  Use mask                : " << fn_mask << endl;
        if (fn_solv != "")
            cerr << "  Solvent mask            : " << fn_solv << endl;
        if (fn_solv2 != "")
            cerr << "  Second solvent mask     : " << fn_solv2 << endl;
        if (fn_frac != "")
        {
            cerr << "  Initial model frections : " << fn_frac << endl;
        }
        cerr << "  Limited shift search    : " << search_shift << endl;
        if (fix_fractions)
        {
            cerr << "  -> Do not update estimates of model fractions." << endl;
        }
        if (fix_sigma_noise)
        {
            cerr << "  -> Do not update sigma-estimate of noise." << endl;
        }
        if (fix_sigma_offset)
        {
            cerr << "  -> Do not update sigma-estimate of origin offsets." << endl;
        }
        if (lowres > 0. || highres > 0.)
        {
            cerr << "  -> Limit to resolutions between " << lowres << " and " << highres << endl;
        }
        cerr << " -----------------------------------------------------------------" << endl;
    }

}

// Usage ===================================================================
void Prog_mlf_tomo_prm::usage()
{
    cerr << "Usage:  mlf_tomo [options] " << endl;
    cerr << "   -i <selfile>                : Selfile with input images \n";
    cerr << "   -ref <selfile/image>        : Selfile with initial reference images/single reference image \n";
    cerr << " [ -o <rootname=\"mlf_tomo\"> ] : Output rootname \n";
    cerr << " [ -sumwedges <selfile=\"\"> ]  : Selfile with sum of wedges for each reference \n";
    cerr << " [ -wedgelist <docfile=\"\"> ]  : Docfile with wedge parameters \n";

    cerr << " [ -offset <float=0>]          : Search radius for limited translations [pix] \n";
    cerr << " [ -max_shift <float=dim/2>]   : Maximum allowed shift [pix] \n";
    cerr << "   Angular search parameters:        \n";
    cerr << " [ -rot  <rot0=0>  <rotF=0>  <step_rot=1> \n";
    cerr << " [ -tilt <tilt0=0> <tiltF=0> <step_tilt=1>\n";
    cerr << " [ -psi  <psi0=0>  <psiF=0>  <step_psi=1> \n";
    cerr << " [ -sym <symfile> ]            : Enforce symmetry \n";
    cerr << " [ -more_options ]             : Show all possible input parameters \n";
}

// Extended usage ===================================================================
void Prog_mlf_tomo_prm::extended_usage()
{
    cerr << "Additional options: " << endl;
    cerr << " [ -iter <int=100> ]           : Maximum number of iterations to perform \n";
    cerr << " [ -frac <docfile=\"\"> ]        : Docfile with expected model fractions (default: even distr.)\n";
    cerr << " [ -fix_sigma_noise]           : Do not re-estimate the standard deviation in the pixel noise \n";
    cerr << " [ -fix_sigma_offset]          : Do not re-estimate the standard deviation in the offsets \n";
    cerr << " [ -fix_fractions]             : Do not re-estimate the model fractions \n";
    cerr << endl;
    exit(1);
}

// This routine is for SF-independent side-info calculations
void Prog_mlf_tomo_prm::produce_Side_info()
{

    FileName fn_vol, fn_tmp;
    VolumeXmipp vol;
    headerXmipp head;
    Matrix3D<double> Maux, Faux_real, Faux_imag;
    vector<Matrix3D<double> > Vdum;
    Matrix3D<complex<double> > Faux, Fref;
    int xdim, ydim, zdim, c, iaux, ifound, refno = 0;
    float xx, yy;
    double dum, avg;

    nr_tilt = 0;
    nr_rot_tilt = 0;
    for (double tilt = tilt0; tilt <= tiltF; tilt += tilt_step)
    {
        nr_tilt++;
        if (tilt == 0) nr_rot_tilt += 1;
        else nr_rot_tilt += CEIL(360. * sin(DEG2RAD(tilt)) / rot_step);
    }
    nr_psi = 0;
    for (double psi = psi0; psi <= psiF; psi += psi_step) nr_psi++;

    // Get image size
    SF.go_beginning();
    vol.read(SF.NextImg());
    if (XSIZE(vol()) != YSIZE(vol())) REPORT_ERROR(1, "ERROR% unequal dimensions: Only cubic volumes are allowed!");
    if (XSIZE(vol()) != ZSIZE(vol())) REPORT_ERROR(1, "ERROR% unequal dimensions: Only cubic volumes are allowed!");
    dim = XSIZE(vol());
    dim3 = (double)dim * dim * dim;
    Faux_real.resize(dim, dim, dim);
    Faux_imag.resize(dim, dim, dim);

    // Make relevant masks
    mask.resize(dim, dim, dim);
    mask.setXmippOrigin();
    outside_mask.resize(dim, dim, dim);
    outside_mask.setXmippOrigin();
    BinarySphericalMask(mask, dim / 2, INNER_MASK);
    BinarySphericalMask(outside_mask, dim / 2, OUTSIDE_MASK);

    // Read image- and reference- selfiles
    if (Is_VolumeXmipp(fn_ref))
    {
        SFr.reserve(1);
        SFr.insert(fn_ref);
        SFw.reserve(1);
        SFw.insert(fn_wed);
    }
    else
    {
        SFr.read(fn_ref);
    }

    // Fill limited translation search vector Vtrans
    Matrix1D<double> offsets(3);
    nr_trans = 0;
    FOR_ALL_ELEMENTS_IN_MATRIX3D(Maux)
    {
        double rr = (double)i * i + (double)j * j + (double)k * k;
        if (rr <= search_shift*search_shift)
        {
            offsets(0) = (double)j;
            offsets(1) = (double)i;
            offsets(2) = (double)k;
            if (i == 0 && j == 0 && k == 0)
            {
                zero_trans = nr_trans;
            }
            Vtrans.push_back(offsets);
            nr_trans++;
        }
    }

    // Read in all reference images in memory
    Maux.initZeros(dim, dim, dim);
    Maux.setXmippOrigin();
    nr_ref = 0;
    SFr.go_beginning();
    while (!SFr.eof())
    {
        Fref_trans.push_back(Vdum);
        vol.read(SFr.NextImg());
        vol().setXmippOrigin();
        //computeStats_within_binary_mask(outside_mask,vol(),dum,dum,avg,dum);
        //apply_binary_mask(mask,vol(),vol(),avg);
        //Maux+=vol();
        // Store the centered FFT of each reference
        // Store real and imaginary parts separately!!
        FourierTransform(vol(), Fref);
        CenterOriginFFT(Fref, true);

        // Pre-calculate all translated versions of Fref
        for (int itrans = 0; itrans < nr_trans; itrans++)
        {
            Faux = Fref;
            ShiftFFT(Faux, Vtrans[itrans](0), Vtrans[itrans](1), Vtrans[itrans](2));
            Complex2RealImag(Faux, Faux_real, Faux_imag);
            // Store real and imaginary parts separately!!
            Fref_trans[refno].push_back(Faux_real);
            Fref_trans[refno].push_back(Faux_imag);
        }

        // Default start is all equal model fractions
        alpha_k.push_back((double)1 / SFr.ImgNo());
        nr_ref++;
    }

    // Prepare for smoothing RETHINK THIS WITH CENTERORIGINFFT!!
    /*
    if (theta0>0) theta=theta0;
    if (theta>0) 
    {
    double theta_corr=(double)nr_img/(double)(nr_ref*nr_ref);
    for (int refno=0;refno<nr_ref; refno++) 
    {
     Iref[refno]+=(theta*theta_corr)*Maux;
     Iref[refno]/=(theta*theta_corr+1);
    }
    }
    */

    // Read in symmetry information
    if (fn_sym != "") SL.read_sym_file(fn_sym);

    // read in model fractions if given on command line (else uniform distribution)
    if (fn_frac != "")
    {
        DocFile  DF;
        DocLine DL;
        DF.read(fn_frac);
        DF.go_first_data_line();
        double sumfrac = 0.;
        for (refno = 0; refno < nr_ref; refno++)
        {
            DL = DF.get_current_line();
            alpha_k[refno] = DL[0];
            sumfrac += alpha_k[refno];
            DF.next_data_line();
        }
        if (ABS(sumfrac - 1.) > 1e-4)
            cerr << " ->WARNING: Sum of all expected model fractions (" << floatToString(sumfrac) << ") is not one!" << endl;
        for (refno = 0; refno < nr_ref; refno++)
        {
            alpha_k[refno] /= sumfrac;
        }
    }

    // Store angles for missing wedges
    nr_wedge = 0;
    if (fn_wlist != "")
    {
        wedgelist ww;
        DocFile DF1;
        DF1.read(fn_wlist);
        DF1.go_beginning();
        while (!DF1.eof())
        {
            ww.num = ROUND(DF1(0));
            ww.th0 = (double)DF1(1);
            ww.thF = (double)DF1(2);
            wedges.push_back(ww);
            nr_wedge++;
            if (debug) cerr << "wedge " << ww.num << " = " << ww.th0 << " - " << ww.thF << endl;
            DF1.next();
        }
        DF1.clear();
    }

    // Make resolution mask
    resmask.resize(dim, dim, dim);
    resmask.setXmippOrigin();
    resmask2.resize(dim, dim, dim);
    resmask2.setXmippOrigin();
    if (highres > 0.5) REPORT_ERROR(1, "Highres should be lower than 0.5!");
    if (highres < 0) highres = 0.5;
    if (lowres > 0.5) REPORT_ERROR(1, "Lowres should be lower than 0.5!");
    if (lowres < 0) lowres = 0;
    resol_max = ROUND(highres * dim);
    resol_min = ROUND(lowres * dim);
    BinarySphericalMask(resmask2, resol_max, INNER_MASK);
    BinarySphericalMask(resmask, resol_min, INNER_MASK);
    resmask = resmask2 - resmask;

    // Initialize Vsigma2
    Matrix3D<double> Mwedge(dim, dim, dim);
    Mwedge.setXmippOrigin();
    Vsigma2.clear();
    for (int i = 0; i < resol_max; i++)
    {
        Vsigma2.push_back(0.);
    }
    if (fn_sig != "")
    {
        ifstream fh;
        fh.open((fn_sig).c_str(), ios::in);
        for (int i = 0; i < resol_max; i++)
        {
            fh >> iaux;
            fh >> Vsigma2[i];
        }
        fh.close();
    }

    // Create the initial pointers
    update_pointers();

}

// This routine is for (splitted) SF-dependent side-info calculations
void Prog_mlf_tomo_prm::produce_Side_info2()
{

    FileName fn_vol;
    int iaux, ifound, imgno;

    // Store tomogram angles, offset vectors and missing wedge
    // parameters for all particles
    if (fn_doc != "")
    {
        DocFile DF;
        DF.read(fn_doc);

        SF.go_beginning();
        while (!SF.eof())
        {
            fn_vol = SF.NextImg();
            if (DF.search_comment(fn_vol))
            {
                img_rot.push_back(DF(0));
                img_tilt.push_back(DF(1));
                img_psi.push_back(DF(2));
                img_xoff.push_back((double)ROUND(DF(3)));
                img_yoff.push_back((double)ROUND(DF(4)));
                img_zoff.push_back((double)ROUND(DF(5)));
                if (nr_wedge > 0)
                {
                    img_wednr.push_back(DF(6));
                    iaux = ROUND(DF(6));
                    ifound = 0;
                    for (int iw = 0; iw < nr_wedge; iw++)
                    {
                        if (iaux == wedges[iw].num)
                        {
                            img_th0.push_back(wedges[iw].th0);
                            img_thF.push_back(wedges[iw].thF);
                            ifound++;
                        }
                    }
                    if (ifound != 1)
                    {
                        cerr << ifound;
                        cerr << "ERROR% wedge " << iaux << " for tomogram " << fn_vol << " not found in wedge-list" << endl;
                        exit(0);
                    }
                }
                else
                {
                    img_wednr.push_back(0.);
                    img_th0.push_back(0.);
                    img_thF.push_back(0.);
                }
            }
            else
            {
                cerr << "ERROR% " << fn_vol << " not found in document file" << endl;
                exit(0);
            }
        }
    }
    else
    {
        SF.go_beginning();
        while (!SF.eof())
        {
            SF.NextImg();
            img_rot.push_back(0.);
            img_tilt.push_back(0.);
            img_psi.push_back(0.);
            img_xoff.push_back(0.);
            img_yoff.push_back(0.);
            img_zoff.push_back(0.);
            img_wednr.push_back(0.);
            img_th0.push_back(0.);
            img_thF.push_back(0.);
        }
    }

    // Planned misalignment  (for testing purposes...)
    if (fn_misalign != "")
    {
        double rot, tilt, psi, mis_rot, mis_tilt, mis_psi, mis_xoff, mis_yoff, mis_zoff;
        Matrix2D<double> A_img(4, 4), A_mis(4, 4);
        DocFile DFmis;

        DFmis.read(fn_misalign);
        SF.go_beginning();
        imgno = 0;
        while (!SF.eof())
        {
            fn_vol = SF.NextImg();
            if (DFmis.search_comment(fn_vol))
            {
                mis_rot = DFmis(0);
                mis_tilt = DFmis(1);
                mis_psi = DFmis(2);
                mis_xoff = DFmis(3);
                mis_yoff = DFmis(4);
                mis_zoff = DFmis(5);
            }
            else
            {
                cerr << "ERROR% " << fn_vol << " not found in misalignment document file" << endl;
                exit(0);
            }
            A_img = Euler_rotation3DMatrix(img_rot[imgno], img_tilt[imgno], img_psi[imgno]);
            A_mis = Euler_rotation3DMatrix(mis_rot, mis_tilt, mis_psi);
            A_img = A_mis * A_img;
            A_img.resize(3, 3);
            Euler_matrix2angles(A_img, mis_rot, mis_tilt, mis_psi);
            img_rot[imgno] = mis_rot;
            img_tilt[imgno] = mis_tilt;
            img_psi[imgno] = mis_psi;
            img_xoff[imgno] += mis_xoff;
            img_yoff[imgno] += mis_yoff;
            img_zoff[imgno] += mis_zoff;
            imgno++;
        }
    }

}

// Estimate initial sigma2 for fourier-mode from power spectra of volumes
void Prog_mlf_tomo_prm::estimate_initial_sigma2()
{

    VolumeXmipp      vol;
    Matrix3D<complex<double> > Faux;
    DocFile          DF;
    FileName         fn_vol, fn_out;
    int              ii, ires, iwedge, imgno, c, nn;
    double           tmpr, tmpi;
    vector<double>   Vsum, Vavg_real, Vavg_imag;

    if (istart == 1)
    { // else assume the files have been read with fixed filenames, and do nothing

        if (verb > 0)
        {
            cerr << "  Estimating initial noise models from average power spectra" << endl;
            nn = SF.ImgNo();
            init_progress_bar(nn);
            c = MAX(1, nn / 60);
        }

        Vsigma2.clear();
        Vsum.clear();
        Vavg_real.clear();
        Vavg_imag.clear();
        for (int i = 0; i < resol_max; i++)
        {
            Vsigma2.push_back(0.);
            Vsum.push_back(0.);
            Vavg_real.push_back(0.);
            Vavg_imag.push_back(0.);
        }
        if (fn_doc != "" && nr_wedge > 0)
        {
            DF.read(fn_doc);
            SF.go_beginning();
            imgno = 0;
            while (!SF.eof())
            {
                fn_vol = SF.NextImg();
                vol.read(fn_vol);
                vol().setXmippOrigin();
                if (DF.search_comment(fn_vol))
                {
                    iwedge = ROUND(DF(6)) - 1;
                    FourierTransform(vol(), Faux);
                    CenterOriginFFT(Faux, true);
                    for (int ipoint = 0; ipoint < nr_pointer[iwedge]; ipoint++)
                    {
                        ii = pointer[iwedge][ipoint];
                        ires = pointer_resol[iwedge][ipoint];
                        tmpr = (double)((Faux).data[ii]).real();
                        tmpi = (double)((Faux).data[ii]).imag();
                        Vavg_real[ires] += tmpr;
                        Vavg_imag[ires] += tmpi;
                        Vsigma2[ires] += tmpr * tmpr + tmpi * tmpi;
                        Vsum[ires] += 1.;
                    }
                }
                else REPORT_ERROR(1, fn_vol + " not found in document file");
                if (verb > 0) if (imgno % c == 0) progress_bar(imgno);
            }
            if (verb > 0) progress_bar(nn);
            for (int ires = 0; ires < resol_max; ires++)
            {
                if (Vsum[ires] > 0.)
                {
                    Vsigma2[ires] /= 2 * Vsum[ires];
                    /*
                    tmpr= ravg_sumwedge(ires)*Vavg_real[ires];
                    tmpi= ravg_sumwedge(ires)*Vavg_imag[ires];
                    tmpr=(tmpr*tmpr+tmpi*tmpi)/Vsum[ires];
                    Vsigma2[ires]/=Vsum[ires];
                    if (debug)
                    {
                        cerr<<ires<<" Vsig2= "<<Vsigma2[ires]<<" avg2= "<<tmpr<<" sum= "<<Vsum[ires]<<" new sigma2= "<<(Vsigma2[ires]-tmpr)/2<<endl;
                    }
                    Vsigma2[ires]=(Vsigma2[ires]-tmpr)/2;
                    */
                }
                else Vsigma2[ires] = 0.;
            }
            if (verb > 0)
            {
                // Write to disc
                ofstream fh;
                fn_out = fn_root + "_it";
                fn_out.compose(fn_out, istart - 1, "");
                fn_out += "_sigma2.dat";
                fh.open((fn_out).c_str(), ios::out);
                for (int i = 0; i < resol_max; i++)
                {
                    fh << i << " " << Vsigma2[i] << "\n";
                }
                fh.close();
            }

        }
        else REPORT_ERROR(1, "-MLF mode requires the definition of missing wedges and the use of a docfile");
    }

}

// Calculate probability density function of the translations
void Prog_mlf_tomo_prm::calculate_pdf_trans()
{

    double r2, pdfpix, sum;
    pdf_trans.resize(dim, dim, dim);
    pdf_trans.setXmippOrigin();
    Mr2.resize(dim, dim, dim);
    Mr2.setXmippOrigin();

    FOR_ALL_ELEMENTS_IN_MATRIX3D(pdf_trans)
    {
        r2 = (double)(j * j + i * i + k * k);
        if (sigma_offset > 0.)
        {
            pdfpix = exp(-r2 / (2 * sigma_offset * sigma_offset));
            pdfpix /= (double)2 * PI * sigma_offset * sigma_offset * nr_rot_tilt * nr_psi;
        }
        else
        {
            if (k == 0 && j == 0 && i == 0) pdfpix = 1.;
            else pdfpix = 0.;
        }
        VOL_ELEM(pdf_trans, k, i, j) = pdfpix;
        VOL_ELEM(Mr2, k, i, j) = r2;
    }

}

// Make new pointers
void Prog_mlf_tomo_prm::update_pointers()
{

    // MODIFY THIS TO BE ABLE TO CONTROL LOW AND HIGHRES PER ITERATION
    // Then make some SSNR, and a "significant-pointer" as well...

    Matrix2D<double> I(4, 4);
    Matrix3D<double> Mwedge, Maux;
    vector<int> dum;
    vector<double> dum2;
    int iaux;
    VolumeXmipp Vt;
    FileName fnt;
    Vt().resize(dim, dim, dim);

    pointer.clear();
    pointer_mis.clear();
    pointer_i.clear();
    pointer_j.clear();
    pointer_k.clear();
    pointer_resol.clear();
    nr_pointer.clear();
    nr_pointer_mis.clear();
    nonzero_pixels.clear();
    Mwedge.resize(dim, dim, dim);
    Mwedge.setXmippOrigin();
    I.initIdentity();

    // Get a resolution pointer in Fourier-space
    Mresol.resize(dim, dim, dim);
    Mresol.setXmippOrigin();
    FOR_ALL_ELEMENTS_IN_MATRIX3D(Mresol)
    {
        VOL_ELEM(Mresol, k, i, j) = ROUND(sqrt((double)(i * i + j * j + k * k)));
    }

    if (debug)
    {
        FOR_ALL_DIRECT_ELEMENTS_IN_MATRIX3D(Vt())
        {
            dVkij(Vt(), k, i, j) = (double)dVkij(Mresol, k, i, j);
        }
        Vt.write("Mresol.vol");
    }

    // Make pointers to all non-zero pixels in all wedges
    for (int iwedge = 0; iwedge < nr_wedge; iwedge++)
    {
        nonzero_pixels.push_back(dum2);
        for (int i = 0; i < resol_max; i++)
        {
            nonzero_pixels[iwedge].push_back(0.);
        }
        BinaryWedgeMask(Mwedge, wedges[iwedge].th0, wedges[iwedge].thF, I);
        if (debug)
        {
            FOR_ALL_DIRECT_ELEMENTS_IN_MATRIX3D(Vt())
            {
                dVkij(Vt(), k, i, j) = (double)dVkij(Mwedge, k, i, j);
            }
            fnt.compose("Mwedge", iwedge, "vol");
            Vt.write(fnt);
        }
        pointer.push_back(dum);
        pointer_mis.push_back(dum);
        pointer_i.push_back(dum);
        pointer_j.push_back(dum);
        pointer_k.push_back(dum);
        pointer_resol.push_back(dum);
        nr_pointer.push_back(0);
        nr_pointer_mis.push_back(0);
        FOR_ALL_DIRECT_ELEMENTS_IN_MATRIX3D(Mwedge)
        {
            iaux = dVkij(Mresol, k, i, j);
            if (iaux >= resol_min && iaux <= resol_max)
            {
                if (dVkij(Mwedge, k, i, j) > 0)
                {
                    pointer[iwedge].push_back(k*dim*dim + i*dim + j);
                    pointer_i[iwedge].push_back(i);
                    pointer_j[iwedge].push_back(j);
                    pointer_k[iwedge].push_back(k);
                    pointer_resol[iwedge].push_back(iaux);
                    nr_pointer[iwedge]++;
                    nonzero_pixels[iwedge][iaux] += 1.;
                }
                else
                {
                    pointer_mis[iwedge].push_back(k*dim*dim + i*dim + j);
                    nr_pointer_mis[iwedge]++;
                }
            }
        }
        if (debug)
        {
            cerr << "non-zero pixels for wedge" << iwedge;
            for (int i = 0; i < resol_max; i++)
            {
                cerr << nonzero_pixels[iwedge][i] << " ";
            }
            cerr << endl;
        }
    }

}

// Here perform the main probability-weighted integration over all
// rotations, translations and classes of the given image
void Prog_mlf_tomo_prm::MLF_integrate(Matrix3D<double> Mimg, Matrix2D<double> A_img, int iwedge,
                                      vector<Matrix3D<double> > &wsum_Fimgs,
                                      vector<Matrix3D<double> > &wsum_Fweds,
                                      vector<double> &wsum_sigma2, double &wsum_sigma_offset,
                                      vector<double> &sumw, double &LL, double &fracweight,
                                      int &opt_refno, double &opt_rot, double &opt_tilt, double &opt_psi,
                                      double &opt_xoff, double &opt_yoff, double &opt_zoff)
{

    Matrix3D<double> Mrotwedge, Frotref_real, Frotref_imag, Frotimg_real, Frotimg_imag;
    Matrix3D<double> Faux_real, Faux_imag;
    vector<Matrix3D<double> > Fimg_trans;
    vector<Matrix3D<double> > dum;
    vector<double> Vweight;
    Matrix3D<complex<double> > Fimg, Faux;
    Matrix2D<double> A(4, 4), A_rot(4, 4), I(4, 4);
    Matrix1D<double> offsets(3);
    vector<double> radavg_sigma2(resol_max);
    vector<vector<double> > Vwsum_sigma2;
    vector<double> refw(nr_ref), pdf_itrans;
    double sum_refw = 0., maxweight = -99.e99, mindiff = 99.e99, wsum_corr = 0.;
    int iweight, ioptx, iopty, ioptz, xmax, ymax, zmax, opt_itrans, ii, ires;
    double rot, tilt, psi, rot_sam, diff, sumweight, maxw, sum, aux, weight;
    double tmpr, tmpi, tmpd;
    I.initIdentity();

    VolumeXmipp Vt;
    FileName fnt;
    Vt().resize(dim, dim, dim);
    Faux_real.resize(dim, dim, dim);
    Faux_imag.resize(dim, dim, dim);
    Vwsum_sigma2.clear();

    // Calculate centered FFTs of all translated versions of Fimg
    FourierTransform(Mimg, Fimg);
    CenterOriginFFT(Fimg, true);

    // Force missing wedge
    Faux.resize(dim, dim, dim);
    for (int ipoint = 0; ipoint < nr_pointer[iwedge]; ipoint++)
    {
        ii = pointer[iwedge][ipoint];
        (Faux).data[ii] = (Fimg).data[ii];
    }
    Fimg = Faux;

    // Pre-calculate all translated versions of Fimg
    for (int itrans = 0; itrans < nr_trans; itrans++)
    {
        Faux = Fimg;
        ShiftFFT(Faux, Vtrans[itrans](0), Vtrans[itrans](1), Vtrans[itrans](2));
        Complex2RealImag(Faux, Faux_real, Faux_imag);
        // Store real and imaginary parts separately!!
        Fimg_trans.push_back(Faux_real);
        Fimg_trans.push_back(Faux_imag);
        pdf_itrans.push_back(VOL_ELEM(pdf_trans, (int)(Vtrans[itrans](2) + opt_zoff),
                                      (int)(Vtrans[itrans](1) + opt_yoff),
                                      (int)(Vtrans[itrans](0) + opt_xoff)));
    }

    // Calculate all squared differences
    iweight = -1;
    for (int itilt = 0; itilt < nr_tilt; itilt++)
    {
        tilt = tilt0 + (double)itilt * tilt_step;
        if (tilt == 0) nr_rot = 1;
        else nr_rot = CEIL(ABS((rotF - rot0) * sin(DEG2RAD(tilt)) / (rot_step * sin(DEG2RAD(tiltF)))));
        rot_sam = (rotF - rot0) / (double)nr_rot;
        for (int irot = 0; irot < nr_rot; irot++)
        {
            rot = rot0 + (double)irot * rot_sam;
            for (int ipsi = 0; ipsi < nr_psi; ipsi++)
            {
                psi = -rot + psi0 + (double)ipsi * psi_step;
                A_rot = Euler_rotation3DMatrix(rot, tilt, psi);
                A = A_rot * A_img;
                for (int refno = 0;refno < nr_ref; refno++)
                {
                    if (alpha_k[refno] > 0.)
                    {
                        refw[refno] = 0.;
                        applyGeometryBSpline(Frotref_real, A, Fref_trans[refno][2*zero_trans], 3, IS_INV, DONT_WRAP);
                        applyGeometryBSpline(Frotref_imag, A, Fref_trans[refno][2*zero_trans+1], 3, IS_INV, DONT_WRAP);
                        if (debug)
                        {
                            RealImag2Complex(Frotref_real, Frotref_imag, Faux);
                            CenterOriginFFT(Faux, false);
                            InverseFourierTransform(Faux, Vt());
                            Vt.write("Mrotref.vol");
                            RealImag2Complex(Fimg_trans[0], Fimg_trans[1], Faux);
                            CenterOriginFFT(Faux, false);
                            InverseFourierTransform(Faux, Vt());
                            Vt.write("Mimgtrans.vol");
                        }
                        for (int itrans = 0; itrans < nr_trans; itrans++)
                        {
                            iweight++;
                            diff = 0.;
                            for (int i = 0; i < resol_max; i++)
                            {
                                radavg_sigma2[i] = 0.;
                            }
                            if (debug) Vt().initZeros();
                            for (int ipoint = 0; ipoint < nr_pointer[iwedge]; ipoint++)
                            {
                                ii = pointer[iwedge][ipoint];
                                ires = pointer_resol[iwedge][ipoint];
                                tmpr = (Fimg_trans[2*itrans]).data[ii] -
                                       (Frotref_real).data[ii];
                                tmpi = (Fimg_trans[2*itrans+1]).data[ii] -
                                       (Frotref_imag).data[ii];
                                tmpd = tmpr * tmpr + tmpi * tmpi;
                                radavg_sigma2[ires] += tmpd;
                                diff += tmpd / (2 * Vsigma2[ires]);
                                // probably this if is too expensive for a debug...
                                //if (debug) Vt().data[ii]=(double)tmpd/(2*Vsigma2[ires]);
                                if (debug && ii == 13066)
                                {
                                    cerr << "Mrotwedge= " << (Mrotwedge).data[ii] << " Fimg_trans[2*itrans]= " << (Fimg_trans[2*itrans].data[ii]) << " (Frotref_real)= " << (Frotref_real).data[ii] << " diff2= " << tmpd << endl;
                                }
                            }
                            //if (debug) {
                            //  Vt.write("diff.vol");
                            //}
                            Vwsum_sigma2.push_back(radavg_sigma2);
                            Vweight.push_back(diff);
                            if (diff < mindiff) mindiff = diff;
                            if (debug)
                            {
                                cerr << rot << " " << tilt << " " << psi << " " << itrans << " " << diff << " " << mindiff;
                                if (diff == mindiff) cerr << "**" << endl;
                                else cerr << endl;
                            }
                        }
                    }
                }
            }
        }
    }

    // Now that we have mindiff2 calculate all weights, maxweight & sumweight
    iweight = -1;
    for (int itilt = 0; itilt < nr_tilt; itilt++)
    {
        tilt = tilt0 + (double)itilt * tilt_step;
        if (tilt == 0) nr_rot = 1;
        else nr_rot = CEIL(ABS((rotF - rot0) * sin(DEG2RAD(tilt)) / (rot_step * sin(DEG2RAD(tiltF)))));
        rot_sam = 360. / (double)nr_rot;
        for (int irot = 0; irot < nr_rot; irot++)
        {
            rot = rot0 + (double)irot * rot_sam;
            for (int ipsi = 0; ipsi < nr_psi; ipsi++)
            {
                psi = -rot + psi0 + (double)ipsi * psi_step;
                for (int refno = 0;refno < nr_ref; refno++)
                {
                    if (alpha_k[refno] > 0.)
                    {
                        for (int itrans = 0; itrans < nr_trans; itrans++)
                        {
                            iweight++;
                            aux = Vweight[iweight] - mindiff;
                            if (aux > 1000.) aux = 0.;
                            else aux = exp(-aux) * alpha_k[refno] * pdf_itrans[itrans];
                            Vweight[iweight] = aux;
                            refw[refno] += aux;
                            sum_refw += aux;
                            if (aux > maxweight)
                            {
                                maxweight = aux;
                                opt_itrans = itrans;
                                opt_refno = refno;
                                opt_rot = rot;
                                opt_tilt = tilt;
                                opt_psi = psi;
                            }
                        }
                    }
                }
            }
        }
    }

    // Accumulate weighted sums of images, sigma2 and fraction-parameters
    // and normalize them by sum_refw, such that sum over all weights is one!
    for (int refno = 0;refno < nr_ref; refno++)
        sumw[refno] += refw[refno] / sum_refw;
    fracweight = maxweight / sum_refw;

    iweight = -1;
    for (int itilt = 0; itilt < nr_tilt; itilt++)
    {
        tilt = tilt0 + (double)itilt * tilt_step;
        if (tilt == 0) nr_rot = 1;
        else nr_rot = CEIL(ABS((rotF - rot0) * sin(DEG2RAD(tilt)) / (rot_step * sin(DEG2RAD(tiltF)))));
        rot_sam = 360. / (double)nr_rot;
        for (int irot = 0; irot < nr_rot; irot++)
        {
            rot = rot0 + (double)irot * rot_sam;
            for (int ipsi = 0; ipsi < nr_psi; ipsi++)
            {
                psi = -rot + psi0 + (double)ipsi * psi_step;
                A_rot = Euler_rotation3DMatrix(rot, tilt, psi);
                A = A_rot * A_img;
                for (int refno = 0;refno < nr_ref; refno++)
                {
                    if (alpha_k[refno] > 0.)
                    {
                        for (int itrans = 0; itrans < nr_trans; itrans++)
                        {
                            iweight++;
                            weight = Vweight[iweight];
                            if (weight > SIGNIFICANT_WEIGHT_LOW*maxweight)
                            {
                                weight /= sum_refw;
                                if (debug)
                                {
                                    cerr << " weight= " << weight << " sum_refw= " << sum_refw << endl;
                                }
                                // Impute missing wedges for reference values
                                for (int ipoint = 0; ipoint < nr_pointer_mis[iwedge]; ipoint++)
                                {
                                    ii = pointer_mis[iwedge][ipoint];
                                    (Fimg_trans[2*itrans]).data[ii] = (Fref_trans[refno][2*itrans]).data[ii];
                                    (Fimg_trans[2*itrans+1]).data[ii] = (Fref_trans[refno][2*itrans+1]).data[ii];
                                }
                                // weighted sum of rotated and translated images
                                applyGeometryBSpline(Frotimg_real, A, Fimg_trans[2*itrans], 3, IS_NOT_INV, DONT_WRAP);
                                applyGeometryBSpline(Frotimg_imag, A, Fimg_trans[2*itrans+1], 3, IS_NOT_INV, DONT_WRAP);
                                FOR_ALL_DIRECT_ELEMENTS_IN_MATRIX3D(Frotimg_real)
                                {
                                    dVkij(wsum_Fimgs[2*refno], k, i, j) += weight * dVkij(Frotimg_real, k, i, j);
                                    dVkij(wsum_Fimgs[2*refno+1], k, i, j) += weight * dVkij(Frotimg_imag, k, i, j);
                                }
                                // weighted sum for sigma2
                                for (int i = 0; i < resol_max; i++)
                                {
                                    wsum_sigma2[i] += weight * Vwsum_sigma2[iweight][i];
                                    //if (debug) cerr <<"wsum_sigma2+= "<<weight<<" * "<<Vwsum_sigma2[iweight][i]<<endl;
                                }
                                // weighted sum for origin offsets
                                wsum_sigma_offset += weight * ((Vtrans[itrans](0) + opt_xoff) * (Vtrans[itrans](0) + opt_xoff) +
                                                               (Vtrans[itrans](1) + opt_yoff) * (Vtrans[itrans](1) + opt_yoff) +
                                                               (Vtrans[itrans](2) + opt_zoff) * (Vtrans[itrans](2) + opt_zoff));
                            }
                        }
                    }
                }
            }
        }
    }

    // Save the optimal (combined) angles and origin offsets
    A_rot = Euler_rotation3DMatrix(opt_rot, opt_tilt, opt_psi);
    A = A_rot * A_img;
    A.resize(3, 3);
    Euler_matrix2angles(A, opt_rot, opt_tilt, opt_psi);
    opt_xoff = (double)Vtrans[opt_itrans](0);
    opt_yoff = (double)Vtrans[opt_itrans](1);
    opt_zoff = (double)Vtrans[opt_itrans](2);

}



void Prog_mlf_tomo_prm::sum_over_all_images(SelFile &SF,
        vector<Matrix3D<double> > &wsum_Fimgs,
        vector<Matrix3D<double> > &wsum_Fweds,
        vector<double> &sum_nonzero_pixels,
        vector<double> &wsum_sigma2, double &wsum_sigma_offset,
        vector<double> &sumw, double &LL, double &sumcorr, DocFile &DFo)
{

    Matrix3D<double> Mdzero;
    Matrix1D<double> dataline(9), opt_offsets(3), mis_offsets(3);
    Matrix2D<double> A_img(4, 4);
    VolumeXmipp      img;
    FileName         fn_img;
    double           th0, thF, opt_rot, opt_tilt, opt_psi, maxcorr;
    int              nn, imgno, opt_refno, iwedge;
    SelLine          line;

    // Initialize weighted sums to zero
    Mdzero.resize(dim, dim, dim);
    Mdzero.setXmippOrigin();
    sumw.clear();
    LL = 0.;
    wsum_sigma_offset = 0.;
    wsum_Fimgs.clear();
    wsum_Fweds.clear();
    sumcorr = 0.;
    for (int refno = 0;refno < nr_ref; refno++)
    {
        wsum_Fimgs.push_back(Mdzero);
        wsum_Fimgs.push_back(Mdzero);
        wsum_Fweds.push_back(Mdzero);
        sumw.push_back(0.);
    }
    wsum_sigma2.clear();
    sum_nonzero_pixels.clear();
    for (int i = 0; i < resol_max; i++)
    {
        wsum_sigma2.push_back(0.);
        sum_nonzero_pixels.push_back(0.);
    }

    // Pre-calculate the prior of the origin offsets
    calculate_pdf_trans();

    // Loop over all images
    nn = SF.ImgNo();
    if (verb > 0) init_progress_bar(nn);
    imgno = 0;
    SF.go_beginning();
    while ((!SF.eof()))
    {
        // read tomogram from disc
        fn_img = SF.NextImg();
        img.read(fn_img);
        img().setXmippOrigin();

        // apply (wrapped around!) integer translation of pre-orientation
        opt_offsets(0) = (double)ROUND(img_xoff[imgno]);
        opt_offsets(1) = (double)ROUND(img_yoff[imgno]);
        opt_offsets(2) = (double)ROUND(img_zoff[imgno]);
        img().selfTranslate(opt_offsets, WRAP);

        // Get optimal orientation and wedge information
        A_img = Euler_rotation3DMatrix(img_rot[imgno], img_tilt[imgno], img_psi[imgno]);
        th0 = img_th0[imgno];
        thF = img_thF[imgno];
        iwedge = (int)img_wednr[imgno] - 1;

        // Keep track of non-zero pixels for later sigma2 calculation
        for (int i = 0; i < resol_max; i++)
            sum_nonzero_pixels[i] += nonzero_pixels[iwedge][i];

        // Perform integration over all references, rotations and translations
        MLF_integrate(img(), A_img, iwedge, wsum_Fimgs, wsum_Fweds,
                      wsum_sigma2, wsum_sigma_offset, sumw, LL, maxcorr,
                      opt_refno, opt_rot, opt_tilt, opt_psi, opt_offsets(0), opt_offsets(1), opt_offsets(2));

        // Store new optimal alignment parameters in memory
        img_xoff[imgno] += opt_offsets(0);
        img_yoff[imgno] += opt_offsets(1);
        img_zoff[imgno] += opt_offsets(2);
        img_rot[imgno] = opt_rot;
        img_tilt[imgno] = opt_tilt;
        img_psi[imgno] = opt_psi;

        // Output to docfile
        sumcorr += maxcorr;
        dataline(0) = opt_rot;                // rot
        dataline(1) = opt_tilt;               // tilt
        dataline(2) = opt_psi;                // psi
        dataline(3) = img_xoff[imgno];        // Xoff
        dataline(4) = img_yoff[imgno];        // Yoff
        dataline(5) = img_zoff[imgno];        // Zoff
        if (nr_wedge > 0) dataline(6) = img_wednr[imgno]; // missing wedge number
        else dataline(6) = 0.;
        dataline(7) = (double)(opt_refno + 1);   // Ref
        dataline(8) = maxcorr;               // P_max/P_tot or Corr
        DFo.append_comment(img.name());
        DFo.append_data_line(dataline);

        imgno++;
        if (verb > 0) progress_bar(imgno);

    }
    if (verb > 0) progress_bar(nn);

}

// Update all model parameters
void Prog_mlf_tomo_prm::update_parameters(vector<Matrix3D<double> > &wsum_Fimgs,
        vector<Matrix3D<double> > &wsum_Fweds,
        vector<double> &sum_nonzero_pixels,
        vector<double> &wsum_sigma2, double &wsum_sigma_offset,
        vector<double> &sumw, double &sumcorr,
        double &sumw_allrefs, int iter)
{

    VolumeXmipp Vaux, Vaux2;
    Matrix1D<double> rmean_sigma2;
    Matrix1D<int> center(3), radial_count;
    Matrix3D<complex<double> > Faux, Fsum;
    Matrix3D<double> Maux, Msum, Faux_real, Faux_imag;
    double rr, dum, avg, theta_corr, sum_ref = 0.;

    // Pre-calculate sumw_allrefs
    Msum.initZeros(dim, dim, dim);
    Msum.setXmippOrigin();
    sumw_allrefs = 0.;
    for (int refno = 0;refno < nr_ref; refno++)
    {
        sumw_allrefs += sumw[refno];
    }
    sumcorr /= sumw_allrefs;

    VolumeXmipp Vt;
    FileName fnt;

    // Symmetrize wsum_Fimgs and wsum_Fweds (in Fourier space)
    if (fn_sym != "")
    {
        for (int refno = 0;refno < nr_ref; refno++)
        {
            Vaux() = wsum_Fweds[refno];
            symmetrize(SL, Vaux, Vaux2, false, false);
            wsum_Fweds[refno] = Vaux2();
            Vaux() = wsum_Fimgs[2*refno];
            symmetrize(SL, Vaux, Vaux2, false, false);
            wsum_Fimgs[2*refno] = Vaux2();
            Vaux() = wsum_Fimgs[2*refno+1];
            symmetrize(SL, Vaux, Vaux2, false, false);
            wsum_Fimgs[2*refno+1] = Vaux2();
            // remove_artefacts: apply highres mask here?
            // STILL TO INSERT HERE! LETS FIRST SEE EFFECTS WHETHER IT
            // IS NECESSARY!
        }
    }

    // Update all Fref_trans matrices
    for (int refno = 0;refno < nr_ref; refno++)
    {
        if (sumw[refno] > 0.)
        {
            Faux_real = wsum_Fimgs[2*refno] / sumw[refno];
            Faux_imag = wsum_Fimgs[2*refno+1] / sumw[refno];
            RealImag2Complex(Faux_real, Faux_imag, Fsum);

            // Calculate all translated versions of Faux
            for (int itrans = 0; itrans < nr_trans; itrans++)
            {
                Faux = Fsum;
                ShiftFFT(Faux, Vtrans[itrans](0), Vtrans[itrans](1), Vtrans[itrans](2));
                Complex2RealImag(Faux, Faux_real, Faux_imag);
                // Store real and imaginary parts separately!!
                Fref_trans[refno][2*itrans] = Faux_real;
                Fref_trans[refno][2*itrans+1] = Faux_imag;
            }

            // RETHINK SMOOTHING FOR NEW WAY OF DOING STUFF...
            /*
            Msum+=Iref[refno];
            sum_ref+=1.;
            */
        }
        else
        {
            for (int itrans = 0; itrans < nr_trans; itrans++)
            {
                Fref_trans[refno][2*itrans].initZeros();
                Fref_trans[refno][2*itrans+1].initZeros();
            }
        }
    }

    // Smooth references...
    /*
    if (theta>0) {
    theta_corr=(double)nr_img/(double)(nr_ref*nr_ref);
    FourierTransform(Msum,Fsum);
    Fsum*=theta_corr*theta;
    for (int refno=0;refno<nr_ref; refno++) {
     FourierTransform(wsum_Mref[refno],Faux);
     Faux+=Fsum;
     FOR_ALL_DIRECT_ELEMENTS_IN_MATRIX3D(Faux) {
    dVkij(Faux,k,i,j)/=dVkij(wsum_Mwedge[refno],k,i,j)+(double)sum_ref*theta*theta_corr;
     }
     InverseFourierTransform(Faux,Iref[refno]);
    }
    if (theta0>0) 
     theta=theta0*exp(-theta_step*iter);
    else {
     theta-=theta_step;
    }
    theta=MAX(0,theta);
    }
    */

    // Update model fractions
    if (!fix_fractions)
        for (int refno = 0;refno < nr_ref; refno++)
        {
            alpha_k[refno] = sumw[refno] / sumw_allrefs;
        }

    // Update estimate for origin offsets
    if (!fix_sigma_offset)
        sigma_offset = sqrt(wsum_sigma_offset / (3 * sumw_allrefs));

    // Update estimate for noise
    if (!fix_sigma_noise)
    {

        if (theta == 0)
        {
            for (int i = 0; i < resol_max; i++)
            {
                // Factor 2 because of 2D-Gaussian in Fourier space!
                if (sum_nonzero_pixels[i] > 0.)
                    Vsigma2[i] = wsum_sigma2[i] / (2 * sum_nonzero_pixels[i]);
                else Vsigma2[i] = 0.;
                if (debug) cerr << " Vsig2= " << Vsigma2[i] << endl;
            }

        }

        // RETHINK THIS FOR NEW STUFF....
        /*
        else 
        {
            int ires;
            double tmpr,tmpi;
            vector<double> diff2_ref;
            vector<Matrix3D<complex<double> > > Fref;
            Fref.clear();
            for (int i=0; i<resol_max; i++) 
         diff2_ref.push_back(0.);
            for (int refno=0;refno<nr_ref; refno++) 
            {
         FourierTransform(Iref[refno],Faux);
         Complex2RealImag(Faux,Faux_real,Faux_imag);
         Fref.push_back(Faux_real);
         Fref.push_back(Faux_imag);
            }
            for (int refno=0;refno<nr_ref; refno++) 
            {
         if (sumw[refno]>0.) 
         {
             for (int refno2=0; refno2<nr_ref; refno2++) 
             {
          if (refno!=refno2 && sumw[refno2]>0.) 
          {
              FOR_ALL_DIRECT_ELEMENTS_IN_MATRIX3D(Fref[refno]) {
           ires=dVkij(Mresol,k,i,j);
           if (ires>resol_min && ires<resol_max) 
           {
               tmpr= (double)(dVkij(Fref[refno],k,i,j)).real() -
            (double)(dVkij(Fref[refno2],k,i,j)).real();
               tmpi= (double)(dVkij(Fref[refno],k,i,j)).imag() -
            (double)(dVkij(Fref[refno2],k,i,j)).imag();
               diff2_ref[ires]+=tmpr*tmpr+tmpi*tmpi;
           }
              }
          }
             }
         }
            }
            for (int i=0; i<resol_max; i++) 
            {
         // Factor 2 because of 2D-Gaussian in Fourier space!
         if (sum_nonzero_pixels[i]>0.)
             Vsigma2[i]=(wsum_sigma2[i]+diff2_ref[i]*theta*theta_corr)/(2*sum_nonzero_pixels[i]);
         else Vsigma2[i]=0.;
         if (debug) cerr<<" Vsig2= ("<< wsum_sigma2[i]<<" + "<<diff2_ref[i]<<" *" <<theta*theta_corr<<") /2* "<<sum_nonzero_pixels[i]<<endl;
            }
        }
        */
    }

}

// Post-process reference volumes =================================================
void Prog_mlf_tomo_prm::post_process_references(vector<Matrix3D<double> > &Mref)
{

    Matrix3D<complex<double> > Faux;
    Matrix3D<double> Faux_real, Faux_imag, Maux;
    bool changed = false;
    double cutoff = 0.1;

    Faux_real.resize(dim, dim, dim);
    Faux_imag.resize(dim, dim, dim);

    // Calculate real-space references, dividing Fref by Fsumwedge
    Mref.clear();
    for (int refno = 0;refno < nr_ref; refno++)
    {
        RealImag2Complex(Fref_trans[refno][2*zero_trans], Fref_trans[refno][2*zero_trans+1], Faux);
        CenterOriginFFT(Faux, false);
        InverseFourierTransform(Faux, Maux);
        Mref.push_back(Maux);
    }

    // Solvent flattening
    if (fn_solv != "")
    {
        solvent_flattening(Mref, fn_solv);
        changed = true;
    }
    if (fn_solv2 != "")
    {
        solvent_flattening(Mref, fn_solv2);
        changed = true;
    }

    // Masking??


    // If the references have changed, update the reciprocal space vectors
    if (changed)
    {
        for (int refno = 0;refno < nr_ref; refno++)
        {
            FourierTransform(Mref[refno], Faux);
            CenterOriginFFT(Faux, true);
            Complex2RealImag(Faux, Fref_trans[refno][2*zero_trans], Fref_trans[refno][2*zero_trans+1]);
        }
    }

}

// Modify reference volumes ======================================================
void Prog_mlf_tomo_prm::solvent_flattening(vector<Matrix3D<double> > &Mref, FileName &fn_solvent)
{

    VolumeXmipp solv;
    double solvavg, sumsolv;

    solv.read(fn_solvent);
    solv().setXmippOrigin();
    if (XSIZE(solv()) != dim || YSIZE(solv()) != dim || ZSIZE(solv()) != dim)
        REPORT_ERROR(12, "mlf_tomo-solvent_flattening: solvent mask is not of right dimensions");

    solvavg = 0., sumsolv = 0.;
    for (int refno = 0;refno < nr_ref; refno++)
    {
        FOR_ALL_DIRECT_ELEMENTS_IN_MATRIX3D(solv())
        {
            solvavg += dVkij(mask, k, i, j) * dVkij(solv(), k, i, j) * dVkij(Mref[refno], k, i, j);
            sumsolv += dVkij(mask, k, i, j) * dVkij(solv(), k, i, j);
        }
    }
    solvavg /= sumsolv;
    for (int refno = 0;refno < nr_ref; refno++)
    {
        FOR_ALL_DIRECT_ELEMENTS_IN_MATRIX3D(solv())
        {
            dVkij(Mref[refno], k, i, j) -= dVkij(solv(), k, i, j) * (dVkij(Mref[refno], k, i, j) - solvavg);
        }
    }

}

void Prog_mlf_tomo_prm::write_output_files(const int iter, SelFile &SF, DocFile &DF,
        vector<Matrix3D<double> > &Mref,
        double &sumw_allrefs, vector<double> &sumw,
        double &LL, double &avecorr)
{

    FileName fn_tmp, fn_base;
    Matrix1D<double> fracline(1);
    string comment;
    VolumeXmipp tmpvol;
    FourierVolumeXmipp Ftmpvol;


    DF.clear();

    fn_base = fn_root;
    if (iter >= 0)
    {
        fn_base += "_it";
        fn_base.compose(fn_base, iter, "");
    }

    // Write out current reference images and fill log-file
    for (int refno = 0;refno < nr_ref; refno++)
    {
        // Real-space map (sumwedge-corrected!)
        fn_tmp = fn_base + "_ref";
        fn_tmp.compose(fn_tmp, refno + 1, "");
        fn_tmp = fn_tmp + ".vol";
        tmpvol() = Mref[refno];
        tmpvol.write(fn_tmp);

        // Real-space version of original reference
        fn_tmp = fn_base + "_ref";
        fn_tmp.compose(fn_tmp, refno + 1, "");
        fn_tmp = fn_tmp + ".sumvol";
        RealImag2Complex(Fref_trans[refno][2*zero_trans], Fref_trans[refno][2*zero_trans+1], Ftmpvol());
        CenterOriginFFT(Ftmpvol(), false);
        InverseFourierTransform(Ftmpvol(), tmpvol());
        tmpvol.write(fn_tmp);

        // Fill log-file
        fracline(0) = sumw[refno] / sumw_allrefs;
        DF.insert_comment(fn_tmp);
        DF.insert_data_line(fracline);
    }

    // Write out sigma2 for Fourier-mode
    ofstream fh;
    fn_tmp = fn_base + "_sigma2.dat";
    fh.open((fn_tmp).c_str(), ios::out);
    for (int i = 0; i < resol_max; i++)
    {
        fh << i << " " << Vsigma2[i] << "\n";
    }
    fh.close();

    // Write out log-file
    DF.go_beginning();
    comment = "mlf_tomo-logfile: Number of images= " + floatToString(sumw_allrefs);
    comment += " LL= " + floatToString(LL, 10, 5) + " <Pmax/sumP>= " + floatToString(avecorr, 10, 5);
    DF.insert_comment(comment);
    comment = "-noise " + fn_base + "_sigma2.dat -offset " + floatToString(sigma_offset, 10, 7) + " -istart " + integerToString(iter + 1);
    if (theta > 0) comment += " -theta " + floatToString(theta, 6, 3) + " -theta_step " + floatToString(theta_step, 6, 3);
    DF.insert_comment(comment);
    DF.insert_comment("columns: model fraction (1); ");
    fn_tmp = fn_base + ".log";
    DF.write(fn_tmp);

}

