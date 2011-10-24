#!/usr/bin/env xmipp_python
#------------------------------------------------------------------------------------------------
# Protocol for Xmipp-based 2D alignment and classification,
# using maximum-likelihood principles, according to:
#
# Example use:
# ./xmipp_protocol_ml2d.py
#
#  Author:  Sjors Scheres, January 2008
# Updated:  J. M. de la Rosa Trevin July 2011
#
# {begin_of_header}
#   {please_cite}
"""
for ML2D:  Scheres et al. (2005) J.Mol.Biol 348, 139-149
for MLF2D: Scheres et al. (2007) Structure 15, 1167-1177
"""
#{eval} expandCommentRun()

#------------------------------------------------------------------------------------------
# {section} Input
#------------------------------------------------------------------------------------------
# {file}(*.xmd *.stk *.sel){validate}(PathExists) Input images:
""" 
Provide a list of images from a stack or metadata file that make up your data set.
The filenames should be relative to the <ProjectDir> where you are running the <Protocols>
If you have images outside the <Project> you should import them first.
"""
ImgMd = "all_images.xmd"

# Generate references (or classes) ?
""" 
If you set to <No>, you should provide references images (In stack or metadata file). 
The default generation is done by averaging subsets of input images.
"""
DoGenerateReferences = True

# {condition}(DoGenerateReferences) Number of references:
""" Number of references to be generated. """
NumberOfReferences = 3

# {file}{validate}(PathExists){condition}(not DoGenerateReferences) References metadata:
""" Number of references to be generated. """
RefMd = "result_classes.xmd"

#------------------------------------------------------------------------------------------
# {section}{has_question} MLF-specific parameters
#------------------------------------------------------------------------------------------
# Use MLF2D instead of ML2D
DoMlf = False

# Use CTF-amplitude correction inside MLF?
""" 
If set to <Yes>, provide the <ctfdat> file in the field below. 
If set to <No>, provide the images pixel size in Angstrom
"""
DoCorrectAmplitudes = True

# {file}{condition}(DoCorrectAmplitudes and DoMlf) CTFdat file with the input images:
InCtfDatFile = "all_images.ctfdat"

# {condition}(not DoCorrectAmplitudes)Image pixel size (in Angstroms)
PixelSize = 5.6

# Are the images CTF phase flipped?
""" You can run MLF with or without having phase flipped the images.
"""
ImagesArePhaseFlipped = True

# High-resolution limit (in Angstroms)
""" 
No frequencies higher than this limit will be taken into account. 
If zero is given, no limit is imposed
"""
HighResLimit = 20

#------------------------------------------------------------------------------------------
# {section}{has_question} Advanced parameters
#------------------------------------------------------------------------------------------
# Show advanced parameters
AdvancedParameters = False

# Also include mirror in the alignment?
"""  Including the mirror transformation is useful if your particles have a handedness 
and may fall either face-up or face-down on the grid. Note that when you want to use 
this <ML2D> run for later <RCT> reconstruction, you can <NOT include the mirror> 
transformation here.
"""
DoMirror = False

# {condition}(not DoMlf) Use the fast version of this algorithm?
"""
For detaisl see:
<See Scheres et al., Bioinformatics, 21 (Suppl. 2), ii243-ii244>
[http://dx.doi.org/10.1093/bioinformatics/bti1140]
"""
DoFast = True
# {expert}{condition}(DoFast) -C   double
"""
Significance criterion for fast approach  
"""
CFast = 1e-12

# Refine the normalization for each image?
""" 
This variant of the algorithm deals with normalization errors. 
For more info see (and please cite):
<Scheres et. al. (2009) J. Struc. Biol., in press>
"""
DoNorm = False

# Restart after iteration:
""" For previous runs that stopped before convergence, resume the calculations
after the completely finished iteration. (Use zero to start from the beginning)
"""
RestartIter = 0

# {expert} --eps   float
"""
 Stopping criterium 
"""
Eps = 5e-5

# {expert} --iter   int
"""
Maximum number of iterations to perform  
"""
MaxIters = 100

#{expert}  --psi_step   float
"""
In-plane rotation sampling interval (degrees) 
"""
PsiStep = 5.0

#{expert}  --noise   float
"""
Expected standard deviation for pixel noise  
"""
StdNoise = 1

# {expert} --offset   float
"""
Expected standard deviation for origin offset (pixels) 
"""
StdOffset = 3.0

# {expert} --zero_offsets
"""
Kick-start the fast algorithm from all-zero offsets  
"""
ZeroOffsets = False

# {expert} --fix_sigma_noise
"""
 Do not re-estimate the standard deviation in the pixel noise  
"""
FixSigmaNoise = False
# {expert}--fix_sigma_offset
"""
Do not re-estimate the standard deviation in the origin offsets  
"""
FixSigmaOffset = False
# {expert} --fix_fractions
"""
Do not re-estimate the model fractions  
"""
FixFractions = False

#{eval} expandParallel()

#------------------------------------------------------------------------------------------
# {end_of_header} USUALLY YOU DO NOT NEED TO MODIFY ANYTHING BELOW THIS LINE
#------------------------------------------------------------------------------------------------

from protocol_ml2d import *

if __name__ == '__main__':
    protocolMain(ProtML2D)
