/**
 * \author Ali Ghayoor
 * This test file creates an ITK vector image,
 * Creates a NRRD header file for that using DWIMetaDataDictionaryValidator,
 * and writes the image to the disk in NRRD format.

tc
==
.- GetGradient(int)
.- GetGradientCount()
.  SetGradient(int, GradientVectorType)
 - GetMetaDataDictionary()
 - SetMetaDataDictionary(itkMetaDataDictionary)
.- GetGradientTable()
.  SetGradientTable()
.- GetMeasurementFrame()
.  SetMeasurementFrame(MeasurementFrameType)
.  GetDimension()
.  GetSizes()
.  SetSizes(DimensionVectorType, int)
.  GetCenterings()
.  SetCentering(int, str)
.  GetKindsString()
.  GetKinds()
.  SetNonSpatialAxis(int)
RearrangeDimensions(???)
.  GetType()
.  SetType()
.  GetEndian()
.  SetEndian()
.  GetEncoding()
.  SetEncoding()
.  GetBValue()
.  SetBValue(BValueType)
.  GetAnatomicalSpace()
.  SetAnatomicalSpace(AnatomicalSpaceEnum)
GetSpaceDirection()
SetSpaceDirection(SpaceDirectionType)
GetSpaceUnits()
GetSpaceOrigin()
SetSpaceOrigin()
 */
#include <cmath>
#include <cstdio>
#include <itkImage.h>
#include <itkImageFileWriter.h>
#include <itkVectorImage.h>

#include "DWIMetaDataDictionaryValidator.h"

#define KEY_PREFIX "NRRD_"

typedef short                           PixelValue;
typedef itk::VectorImage<PixelValue, 3> ImageType;
typedef std::vector<std::string>        StringVectorType;

ImageType::Pointer CreateVolume()
{
  const int numOfComponents = 8;
  const int componentSize = 10; // each image component has size of componentSize^3

  ImageType::IndexType start;
  start.Fill(0);

  ImageType::SizeType size;
  size.Fill(componentSize);

  ImageType::RegionType region(start, size);

  ImageType::Pointer nrrdVolume = ImageType::New();
  nrrdVolume->SetRegions(region);
  nrrdVolume->SetVectorLength(numOfComponents);
  nrrdVolume->Allocate();

  typedef itk::VariableLengthVector<short> VariableVectorType;
  VariableVectorType variableLengthVector;
  variableLengthVector.SetSize(numOfComponents);
  for( unsigned i = 0; i < numOfComponents; ++i )
     {
     if( i==0 || i==4 )
       {
       variableLengthVector[i] = 0; // assumed as b0 images
       }
     else
       {
       variableLengthVector[i] = i*10; // assumed as 6 gradient components
       }
     }

  nrrdVolume->FillBuffer(variableLengthVector);
  // nrrdVolume = CreateMetaData(nrrdVolume);
  return nrrdVolume;
}

/*
ImageType::Pointer CreateMetaData(ImageType::Pointer nrrdVolume)
{
  nrrdVolume->SetMetaDataDictionary(thisDict);
  ...
  return nrrdVolume
}
*/

int main(int argc, char * argv[])
{
  // Create a vector image
  ImageType::Pointer nrrdVolume = CreateVolume();
  itk::MetaDataDictionary & thisDict = nrrdVolume->GetMetaDataDictionary();
  Nrrd *                   nrrd = nrrdNew();

  try {
    nrrd->dim = 4;
    nrrd->type = nrrdTypeUnknown;
    nrrd->content = "initialized header from DWIMetaDataDictionaryValidator";
    nrrd->spaceDim = 3;
    nrrd->space = nrrdSpaceRightAnteriorSuperior;  // RAS

    nrrd->spaceUnits[0] = "mm";
    nrrd->spaceUnits[1] = "mm";
    nrrd->spaceUnits[2] = "mm";

    nrrd->spaceOrigin[0] = 8.0;
    nrrd->spaceOrigin[1] = 8.0;
    nrrd->spaceOrigin[2] = 8.0;


    nrrd->measurementFrame[0][0] =  0.0;
    nrrd->measurementFrame[0][1] = -1.0;
    nrrd->measurementFrame[0][2] =  0.0;

    nrrd->measurementFrame[1][0] =  1.0;
    nrrd->measurementFrame[1][1] =  0.0;
    nrrd->measurementFrame[1][2] =  0.0;

    nrrd->measurementFrame[2][0] =  0.0;
    nrrd->measurementFrame[2][1] =  0.0;
    nrrd->measurementFrame[2][2] =  1.0;

    NrrdAxisInfo             naxis_x;
    naxis_x.size = 16;
    naxis_x.spacing = 0.5;
    naxis_x.spaceDirection[0] = 1.0;
    naxis_x.spaceDirection[1] = 0.0;
    naxis_x.spaceDirection[2] = 0.0;
    naxis_x.center = nrrdCenterCell;
    naxis_x.kind = nrrdKindSpace;
    naxis_x.label = "x-axis";

    NrrdAxisInfo             naxis_y;
    naxis_y.size = 16;
    naxis_y.spacing = 0.5;
    naxis_y.spaceDirection[0] = 0.0;
    naxis_y.spaceDirection[1] = 1.0;
    naxis_y.spaceDirection[2] = 0.0;
    naxis_y.center = nrrdCenterCell;
    naxis_y.kind = nrrdKindSpace;
    naxis_y.label = "y-axis";

    NrrdAxisInfo             naxis_z;
    naxis_z.size = 16;
    naxis_z.spacing = 0.5;
    naxis_z.thickness = 0.5;
    naxis_z.spaceDirection[0] = 0.0;
    naxis_z.spaceDirection[1] = 0.0;
    naxis_z.spaceDirection[2] = 1.0;
    naxis_z.center = nrrdCenterCell;
    naxis_z.kind = nrrdKindSpace;
    naxis_z.label = "z-axis";

    NrrdAxisInfo             naxis_g;
    naxis_g.size = 8;
    naxis_g.kind = nrrdKindList;
    naxis_g.label = "gradient axis";

    nrrd->axis[0] = naxis_x;
    nrrd->axis[1] = naxis_y;
    nrrd->axis[2] = naxis_z;
    nrrd->axis[3] = naxis_g;

    char *               keyPtr = ITK_NULLPTR;
    char *               valPtr = ITK_NULLPTR;

    for ( unsigned int kvpi = 0; kvpi < nrrdKeyValueSize(nrrd); kvpi++ )
      {
      nrrdKeyValueIndex(nrrd, &keyPtr, &valPtr, kvpi);
      itk::EncapsulateMetaData< std::string >( thisDict, std::string(keyPtr), std::string(valPtr) );
      keyPtr = (char *)airFree(keyPtr);
      valPtr = (char *)airFree(valPtr);
      }

    // Set the number of image dimensions and bail if needed
    unsigned int domainAxisNum, domainAxisIdx[NRRD_DIM_MAX],
      rangeAxisNum, rangeAxisIdx[NRRD_DIM_MAX];
    domainAxisNum = nrrdDomainAxesGet(nrrd, domainAxisIdx);
    rangeAxisNum = nrrdRangeAxesGet(nrrd, rangeAxisIdx);
    // Test variables
    std::vector< std::vector< double > > msrFrame(domainAxisNum);
    // save in MetaDataDictionary those important nrrd fields that
    // (currently) have no ITK equivalent. NOTE that for the per-axis
    // information, we use the same axis index (axii) as in ITK, NOT
    // the original axis index in nrrd (axi).  This is because in the
    // Read() method, non-scalar data is permuted to the fastest axis,
    // on the on the Write() side, its always written to the fastest axis,
    // so we might was well go with consistent and idiomatic indexing.
    NrrdAxisInfo *naxis;
    char                 key[AIR_STRLEN_SMALL];
    const char *         val;
    for ( unsigned int axii = 0; axii < domainAxisNum; axii++ )
      {
      unsigned int axi = domainAxisIdx[axii];
      naxis = nrrd->axis + axi;
      if ( AIR_EXISTS(naxis->thickness) )
        {
        sprintf(key, "%s%s[%u]", KEY_PREFIX,
                airEnumStr(nrrdField, nrrdField_thicknesses), axii);
        itk::EncapsulateMetaData< double >(thisDict, std::string(key),
                                      naxis->thickness);
        }
      if ( naxis->center )
        {
        sprintf(key, "%s%s[%u]", KEY_PREFIX,
                airEnumStr(nrrdField, nrrdField_centers), axii);
        val = airEnumStr(nrrdCenter, naxis->center);
        itk::EncapsulateMetaData< std::string >( thisDict, std::string(key),
                                            std::string(val) );
        }
      if ( naxis->kind )
        {
        sprintf(key, "%s%s[%u]", KEY_PREFIX,
                airEnumStr(nrrdField, nrrdField_kinds), axii);
        val = airEnumStr(nrrdKind, naxis->kind);
        itk::EncapsulateMetaData< std::string >( thisDict, std::string(key),
                                            std::string(val) );
        }
      if ( airStrlen(naxis->label) )
        {
        sprintf(key, "%s%s[%u]", KEY_PREFIX,
                airEnumStr(nrrdField, nrrdField_labels), axii);
        itk::EncapsulateMetaData< std::string >( thisDict, std::string(key),
                                            std::string(naxis->label) );
        }
      }
    if ( airStrlen(nrrd->content) )
      {
      sprintf( key, "%s%s", KEY_PREFIX,
               airEnumStr(nrrdField, nrrdField_content) );
      itk::EncapsulateMetaData< std::string >( thisDict, std::string(key),
                                          std::string(nrrd->content) );
      }
    if ( AIR_EXISTS(nrrd->oldMin) )
      {
      sprintf( key, "%s%s", KEY_PREFIX,
               airEnumStr(nrrdField, nrrdField_old_min) );
      itk::EncapsulateMetaData< double >(thisDict, std::string(key), nrrd->oldMin);
      }
    if ( AIR_EXISTS(nrrd->oldMax) )
      {
      sprintf( key, "%s%s", KEY_PREFIX,
               airEnumStr(nrrdField, nrrdField_old_max) );
      itk::EncapsulateMetaData< double >(thisDict, std::string(key), nrrd->oldMax);
      }
    if ( nrrd->space )
      {
      sprintf( key, "%s%s", KEY_PREFIX,
               airEnumStr(nrrdField, nrrdField_space) );
      val = airEnumStr(nrrdSpace, nrrd->space);

      // keep everything consistent: so enter it as LPS in the meta data
      // dictionary in case it could get converted, otherwise leave it
      // as is

      switch ( nrrd->space )
        {
        case nrrdSpaceRightAnteriorSuperior:
        case nrrdSpaceLeftAnteriorSuperior:
        case nrrdSpaceLeftPosteriorSuperior:
          // in all these cases we could convert
          itk::EncapsulateMetaData< std::string >( thisDict, std::string(key),
                                              std::string( airEnumStr(nrrdSpace, nrrdSpaceLeftPosteriorSuperior) ) );
          break;
        default:
          // we're not coming from a space for which the conversion
          // to LPS is well-defined
          itk::EncapsulateMetaData< std::string >( thisDict, std::string(key),
                                              std::string(val) );
          break;
        }
      }

    double                spacing = 1.0;
    double                spaceDir[NRRD_SPACE_DIM_MAX];
    // spaceDir[0] =
    std::vector< double > spaceDirStd(domainAxisNum);
    int                   spacingStatus;

    int iFlipFactors[3];  // used to flip the measurement frame later on
    for ( unsigned int iI = 0; iI < 3; iI++ )
      {
      iFlipFactors[iI] = 1;
      }
    for ( unsigned int axii = 0; axii < domainAxisNum; axii++ )
      {
      unsigned int naxi = domainAxisIdx[axii];
      spacingStatus = nrrdSpacingCalculate(nrrd, naxi, &spacing, spaceDir);
      spacingStatus = nrrdSpacingStatusNone;
      switch ( spacingStatus )
        {
        case nrrdSpacingStatusNone:
          // Let ITK's defaults stay
          break;
        case nrrdSpacingStatusScalarNoSpace:
          break;
        case nrrdSpacingStatusDirection:
          if ( AIR_EXISTS(spacing) )
            {
            // only set info if we have something to set
            switch ( nrrd->space )
              {
              // on read, convert non-LPS coords into LPS coords, when we can
              case nrrdSpaceRightAnteriorSuperior:
                spaceDir[0] *= -1;   // R -> L
                spaceDir[1] *= -1;   // A -> P
                iFlipFactors[0] = -1;
                iFlipFactors[1] = -1;
                break;
              case nrrdSpaceLeftAnteriorSuperior:
                spaceDir[0] *= -1;   // R -> L
                iFlipFactors[0] = -1;
                break;
              case nrrdSpaceLeftPosteriorSuperior:
                // no change needed
                break;
              default:
                // we're not coming from a space for which the conversion
                // to LPS is well-defined
                break;
              }

            for ( unsigned int saxi = 0; saxi < nrrd->spaceDim; saxi++ )
              {
              spaceDirStd[saxi] = spaceDir[saxi];
              }
            }
          break;
        default:
        case nrrdSpacingStatusUnknown:
          itkGenericExceptionMacro("ReadImageInformation: Error interpreting "
                                   "nrrd spacing (nrrdSpacingStatusUnknown)");
          break;
        case nrrdSpacingStatusScalarWithSpace:
          itkGenericExceptionMacro("ReadImageInformation: Error interpreting "
                            "nrrd spacing (nrrdSpacingStatusScalarWithSpace)");
          break;
        }
      }

    if ( AIR_EXISTS(nrrd->measurementFrame[0][0]) )
      {
      sprintf( key, "%s%s", KEY_PREFIX,
               airEnumStr(nrrdField, nrrdField_measurement_frame) );

      // flip the measurement frame here if we have to
      // so that everything is consistent with the ITK LPS space directions
      // but only do this if we have a three dimensional space or smaller

      for ( unsigned int saxi = 0; saxi < domainAxisNum; saxi++ )
        {
        msrFrame[saxi].resize(domainAxisNum);
        for ( unsigned int saxj = 0; saxj < domainAxisNum; saxj++ )
          {
          if ( domainAxisNum <= 3 )
            {
            msrFrame[saxi][saxj] = iFlipFactors[saxj] * nrrd->measurementFrame[saxi][saxj];
            }
          else
            {
            msrFrame[saxi][saxj] = nrrd->measurementFrame[saxi][saxj];
            }
          }
        }
      itk::EncapsulateMetaData< std::vector< std::vector< double > > >(thisDict,
                                                                  std::string(key),
                                                                  msrFrame);
      }
    else {
      for ( unsigned int saxi = 0; saxi < domainAxisNum; saxi++ )
        {
        msrFrame[saxi].resize(domainAxisNum);
        for ( unsigned int saxj = 0; saxj < domainAxisNum; saxj++ )
          {
          if ( domainAxisNum <= 3 )
            {
            msrFrame[saxi][saxj] = nrrd->measurementFrame[saxi][saxj];
            }
          else
            {
            msrFrame[saxi][saxj] = nrrd->measurementFrame[saxi][saxj];
            }
          }
        }
      itk::EncapsulateMetaData< std::vector< std::vector< double > > >(thisDict,
                                                                  std::string(key),
                                                                  msrFrame);
      }
  // Set the gradient table
  DWIMetaDataDictionaryValidator::GradientTableType          gradients(5);
  gradients[0][0] = 0;     gradients[0][1] = 0;        gradients[0][2] = 0;
  gradients[1][0] = 1.0;   gradients[1][1] = 0.33;     gradients[1][2] = 45;
  gradients[2][0] = -1.0;  gradients[2][1] = -1.03;    gradients[2][2] = -7;
  gradients[3][0] = 300;   gradients[3][1] = -300;     gradients[3][2] = -3;
  gradients[4][0] = -0.24; gradients[4][1] = 0.000;    gradients[4][2] = 2;
  for( unsigned int i = 0; i < 5; i++ )
    {
    char               tmpStr[64];
    sprintf(tmpStr, "DWMRI_gradient_%04u", i);
    std::string        tmpkey(tmpStr);
    sprintf(tmpStr, " %lf %lf %lf", gradients[i][0], gradients[i][1], gradients[i][2]);
    std::string        value(tmpStr);
    itk::EncapsulateMetaData<std::string>(thisDict, tmpkey, value);
    }
  // Set the b-value
  char               tmpStr[64];
  double             bvalue = 1234.5;
  sprintf(tmpStr, " %f", bvalue);
  std::string        value(tmpStr);
  itk::EncapsulateMetaData<std::string>(thisDict, "DWMRI_b-value", value);

/*
    nrrdNix(nrrd);
    }
  catch (...)
    {
    // clean up from an exception
    nrrdNix(nrrd);
    // rethrow exception
    throw;
    }
*/

  // Test DWIMetaDataDictionaryValidator
  DWIMetaDataDictionaryValidator   myValidator;
  itk::MetaDataDictionary          emptyDict;
  // *** GetMetaDataDictionary ***
  std::cout << "*** Test: GetMetaDataDictionary ";
  itk::MetaDataDictionary          myDict = myValidator.GetMetaDataDictionary();
  if (myDict.GetKeys() != emptyDict.GetKeys())
    {
    std::cout << "Failed" << std::endl;
    }
  else { std::cout << "passed!" << std::endl; }
  // *** SetMetaDataDictionary ***
  std::cout << "*** Test: SetMetaDataDictionary ";
  myValidator.SetMetaDataDictionary(thisDict);
  myDict = myValidator.GetMetaDataDictionary();
  if (myDict.GetKeys() != thisDict.GetKeys())
    {
    std::cout << "Failed" << std::endl;
    }
  else { std::cout << "passed!" << std::endl; }
  /*  Print keys and values?
  std::vector<std::string> keys = myDict.GetKeys();
  thisDict.Print(std::cout);
  std::string  outval;
  itk::ExposeMetaData<std::string>(thisDict, "Nrrd_space", outval);
  std::cout << outval << std::endl;
  std::cout << "----------------" << std::endl;
  for (std::vector<std::string>::iterator it = keys.begin() ; it != keys.end(); ++it)
    std::cout << ' ' << *it << " - " << myDict[*it] << std::endl;
  */
  // *** GetDimension ***
  std::cout << "*** Test: GetNumberOfDimensions ";
  try
    {
    myValidator.GetNumberOfDimensions();
    std::cout << "failed!" << std::endl;
    }
  catch (itk::ExceptionObject)
    {
    std::cout << "passed!" << std::endl;
    }
  /*  Currently, metadata in ITK doesn't contain dimensions (stored in itk::Image)
  if (myValidator.GetNumberOfDimensions() != 4)  { std::cout << "failed!" << std::endl; }
  else { std::cout << "passed!" << std::endl; }
  */
  // *** SetNumberOfDimensions ***
  std::cout << "*** Test: SetNumberOfDimensions ";
  try
    {
    myValidator.SetNumberOfDimensions(3);
    std::cout << "failed!" << std::endl;
    }
  catch (itk::ExceptionObject)
    {
    std::cout << "passed!" << std::endl;
    }
  // *** GetMeasurementFrame ***
  std::string                       msrKey = "NRRD_measurement frame";
  std::cout << "*** Test: GetMeasurementFrame ";
  if (myValidator.GetMeasurementFrame() != msrFrame) { std::cout << "failed!" << std::endl; }
  else { std::cout << "passed!" << std::endl; }
  // *** SetMeasurementFrame ***
  std::cout << "*** Test: SetMeasurementFrame ";
  std::vector<std::vector<double> >     newFrame = msrFrame;
  newFrame[1][1] = 3.0;
  myValidator.SetMeasurementFrame(newFrame);
  if (myValidator.GetMeasurementFrame() == msrFrame) { std::cout << "failed!" << std::endl; }
  else { std::cout << "passed!" << std::endl; }
  // *** GetGradient ***
  std::cout << "*** Test: GetGradient ";
  bool is_get_gradient_passing = true;
  int  gradients_size = gradients.size();
  for (int i=0; i < gradients_size; i++)
    {
    if (gradients[i] != myValidator.GetGradient(i))
      {
      //DEBUG: std::cout << "failed! (" << i << ")" << std::endl;
      is_get_gradient_passing = false;
      }
    }
  if (is_get_gradient_passing) { std::cout << "passed!" << std::endl; }
  else { std::cout << "failed!" << std::endl; }
  // *** GetGradientCount ***
  std::cout << "*** Test: GetGradientCount ";
  if (gradients_size != myValidator.GetGradientCount())  { std::cout << "failed!" << std::endl; }
  else { std::cout << "passed!" << std::endl; }
  // *** GetGradientTable ***
  std::cout << "*** Test: GetGradientTable ";
  if (gradients != myValidator.GetGradientTable()) { std::cout << "failed!" << std::endl; }
  else { std::cout << "passed!" << std::endl; }
  // *** SetGradient ***
  std::cout << "*** Test: SetGradient ";
  std::array<double, 3>      newgradient;
  newgradient[0] = 0.707;  newgradient[1] = -0.707;  newgradient[2] = 0.111;
  myValidator.SetGradient(3, newgradient);
  if (newgradient != myValidator.GetGradient(3))
  { std::cout << "failed!" << std::endl; }
  else { std::cout << "passed!" << std::endl; }
  // *** SetGradientTable ***
  std::cout << "*** Test: SetGradientTable ";
  std::vector<std::array<double, 3> >          newgradients(4);
  newgradients[0][0] = 1;  newgradients[0][1] = 2;  newgradients[0][2] = 3;
  newgradients[1][0] = 4;  newgradients[1][1] = 5;  newgradients[1][2] = 6;
  newgradients[2][0] = 7;  newgradients[2][1] = 8;  newgradients[2][2] = 9;
  newgradients[3][0] = 10; newgradients[3][1] = 11; newgradients[3][2] = 12;
  myValidator.SetGradientTable(newgradients);
  if (newgradients != myValidator.GetGradientTable()) { std::cout << "failed!" << std::endl; }
  else { std::cout << "passed!" << std::endl; }
  // *** GetBValue ***
  std::cout << " *** Test: GetBValue ";
  if (myValidator.GetBValue() != bvalue ) {std::cout << "failed!" << std::endl; }
  else { std::cout << "passed!" << std::endl; }
  // *** SetBValue ***
  std::cout << " *** Test: SetBValue ";
  myValidator.SetBValue(500);
  if (myValidator.GetBValue() != 500 ) {std::cout << "failed!" << std::endl; }
  else { std::cout << "passed!" << std::endl; }


/*
  std::string                           sizeKey = "sizes";  // TODO: guess
  std::vector<double>                   sizes;
  sizes[0] = 256;
  sizes[1] = 256;
  sizes[2] = 36;
  sizes[3] = 14;
  // itk::EncapsulateMetaData<std::vector<double> >(thisDict, sizeKey, sizes);
  std::string                           centerKey = "centerings";  // TODO: guess
  std::vector<std::string>              centers(4, "cell");
  centers[3] = "none";
  // itk::EncapsulateMetaData<std::vector<double> >(thisDict, centerKey, centers);
  std::string                           thickKey = "thicknesses";  // TODO: guess
  std::vector<float>                    thicks(4, NAN);
  thicks[2] = 3.0;
  // itk::EncapsulateMetaData<std::vector<double> >(thisDict, thickKey, thicks);
  std::string                           kindsKey = "kinds";  // TODO: guess
  std::vector<std::string>              volume_interleaved(4, "space");
  std::vector<std::string>              slice_interleaved(4, "space");
  std::vector<std::string>              pixel_interleaved(4, "space");
  volume_interleaved[3] = "list";
  slice_interleaved[2] = "list";
  pixel_interleaved[0] = "list";
  // itk::EncapsulateMetaData<std::vector<std::string> >(thisDict, kindsKey, volume_interleaved);

  // itk::EncapsulateMetaData<std::string>(thisDict, "type", "short");
  // itk::EncapsulateMetaData<std::string>(thisDict, "endian", "big");
  // itk::EncapsulateMetaData<std::string>(thisDict, "encoding", "raw");
  // itk::EncapsulateMetaData<int>(thisDict, "b-value", 1000);
  std::string                           RAS = "right-anterior-superior";
  std::string                           LPS = "left-posterior-superior";
  // itk::EncapsulateMetaData<std::string>(thisDict, "space", RAS);

  std::vector<std::vector<double> >     spaceDir(3);
  vnl_matrix_fixed<double, 3, 3>        identity;
  identity.set_identity();

  char                                  tmp[8];
  std::string                           spaceDirString;
  for( unsigned int i = 0; i < 3; i++ )
    {
    spaceDirString += "(";
    spaceDir.resize(3);
    for( unsigned int j = 0; j < 3; j++ )
      {
      sscanf(tmp, "%lf", identity[i][j]);
      spaceDirString += tmp;
      spaceDir[i][j] = identity[i][j];
      }
    spaceDirString += ") ";
    }
  spaceDirString += "none";  // volume interleaved
  // itk::EncapsulateMetaData<std::string >(thisDict, "space direction", spaceDirString);
  // myValidator.SetMetaDataDictionary(thisDict);

  // *** GetSizes ***
  std::cout << " *** Test: GetSizes ";
  if (myValidator.GetSizes() != sizes)
    { std::cout << "failed!" << std::endl; }
  else { std::cout << "passed!" << std::endl; }

  // *** GetCenterings ***
  std::cout << " *** Test: GetCenterings ";
  if (myValidator.GetCenterings() != centers)
    { std::cout << "failed!" << std::endl; }
  else { std::cout << "passed!" << std::endl; }

  // *** SetCentering ***
  std::cout << " *** Test: SetCentering ";
  bool is_set_centerings_passing = true;
  for (unsigned int i=0; i<3; i++)
    {
    centers[i] = "node";
    myValidator.SetCentering(i, "node");
    if (myValidator.GetCenterings() != centers)
      {
      is_set_centerings_passing = false;
      std::cout << "index " << i << "failed!" << std::endl;
      break;
      }
    }
  if (is_set_centerings_passing) { std::cout << "passed!" << std::endl; }

  // *** GetThickness ***
  std::cout << " *** Test: GetThickness ";
  if (myValidator.GetThickness()[2] != thicks[2] ||
      ! isnan(myValidator.GetThickness()[0]) ||
      ! isnan(myValidator.GetThickness()[1]) ||
      ! isnan(myValidator.GetThickness()[2]) )
    { std::cout << "failed!" << std::endl; }
  else { std::cout << "passed!" << std::endl; }

  // *** SetThickness ***
  myValidator.SetThickness(4.5);
  std::cout << " *** Test: SetThickness ";
  if (myValidator.GetThickness()[2] != 4.5 ||
      ! isnan(myValidator.GetThickness()[0]) ||
      ! isnan(myValidator.GetThickness()[1]) ||
      ! isnan(myValidator.GetThickness()[2]) )
    { std::cout << "failed!" << std::endl; }
  else { std::cout << "passed!" << std::endl; }

  // *** GetKinds: volume ***
  std::cout << " *** Test: GetKinds (volume) ";
    if (myValidator.GetKinds() != volume_interleaved) {std::cout << "failed!" << std::endl; }
  else { std::cout << "passed!" << std::endl; }
  // *** GetKindsString: volume ***
  std::cout << " *** Test: GetKindsString (volume) "
  if (myValidator.GetKindsString() != "volume interleaved") {std::cout << "failed!" << std::endl; }
  else { std::cout << "passed!" << std::endl; }

  myValidator.SetNonSpatialAxis(2);  // TODO: myValidator.SetSliceInterleaved()
  // *** GetKinds: slice ***
  std::cout << " *** Test: GetKinds (slice) "
    if (myValidator.GetKinds() != slice_interleaved) {std::cout << "failed!" << std::endl; }
  else { std::cout << "passed!" << std::endl; }
  // *** GetKindsString: slice ***
  std::cout << " *** Test: GetKindsString (slice) "
  if (myValidator.GetKindsString() != "slice interleaved") {std::cout << "failed!" << std::endl; }
  else { std::cout << "passed!" << std::endl; }

  myValidator.SetNonSpatialAxis(0); // TODO: myValidator.SetPixelInterleaved()
  // *** GetKinds (pixel) ***
  std::cout << " *** Test: GetKinds (pixel) "
    if (myValidator.GetKinds() != pixel_interleaved) {std::cout << "failed!" << std::endl; }
  else { std::cout << "passed!" << std::endl; }
  // *** GetKindsString: pixel ***
  std::cout << " *** Test: GetKindsString (pixel) "
  if (myValidator.GetKindsString() != "pixel interleaved") {std::cout << "failed!" << std::endl; }
  else { std::cout << "passed!" << std::endl; }

  myValidator.SetNonSpatialAxis(3); // TODO: myValidator.SetVolumeInterleaved()
  // *** GetKinds ***
  std::cout << " *** Test: GetKinds (volume)(2) "
  if (myValidator.GetKinds() != volume_interleaved) {std::cout << "failed!" << std::endl; }
  else { std::cout << "passed!" << std::endl; }
  // *** GetKindsString: volume (2) ***
  std::cout << " *** Test: GetKindsString(volume)(2) "
  if (myValidator.GetKindsString() != "volume interleaved") {std::cout << "failed!" << std::endl; }
  else { std::cout << "passed!" << std::endl; }

  // *** GetType ***
  std::cout << " *** Test: GetType ";
  if (myValidator.GetType() != "short" ) {std::cout << "failed!" << std::endl; }
  else { std::cout << "passed!" << std::endl; }

  // *** SetType ***
  std::cout << " *** Test: SetType ";
  myValidator.SetType("long");
  if (myValidator.GetType() != "long" ) {std::cout << "failed!" << std::endl; }
  else { std::cout << "passed!" << std::endl; }

  // *** GetEndian ***
  std::cout << " *** Test: GetEndian ";
  if (myValidator.GetEndian() != "big" ) {std::cout << "failed!" << std::endl; }
  else { std::cout << "passed!" << std::endl; }

  // *** SetEndian ***
  std::cout << " *** Test: SetEndian ";
  myValidator.SetEndian("little");
  if (myValidator.GetEndian() != "little" ) {std::cout << "failed!" << std::endl; }
  else { std::cout << "passed!" << std::endl; }

  // *** GetEncoding ***
  std::cout << " *** Test: GetEncoding ";
  if (myValidator.GetEncoding() != "raw" ) {std::cout << "failed!" << std::endl; }
  else { std::cout << "passed!" << std::endl; }

  // *** SetEncoding ***
  std::cout << " *** Test: SetEncoding ";
  myValidator.SetEncoding("ascii");
  if (myValidator.GetEncoding() != "ascii" ) {std::cout << "failed!" << std::endl; }
  else { std::cout << "passed!" << std::endl; }

  // *** GetAnatomicalSpace ***
  std::cout << " *** Test: GetAnatomicalSpace ";
  if (myValidator.GetAnatomicalSpace() != RAS ) {std::cout << "failed!" << std::endl; }
  else { std::cout << "passed!" << std::endl; }

  // *** SetAnatomicalSpace ***
  std::cout << " *** Test: SetAnatomicalSpace (long string) ";
  myValidator.SetAnatomicalSpace(LPS);
  if (myValidator.GetAnatomicalSpace() != LPS ) {std::cout << "failed!" << std::endl; }
  else { std::cout << "passed!" << std::endl; }
  std::cout << " *** Test: SetAnatomicalSpace (short string) ";
  if (myValidator.GetAnatomicalSpace() != RAS ) {std::cout << "failed!" << std::endl; }
  else { std::cout << "passed!" << std::endl; }
  std::cout << " *** Test: SetAnatomicalSpace (short string) ";
  myValidator.SetAnatomicalSpace("LPS");
  if (myValidator.GetAnatomicalSpace() != LPS ) {std::cout << "failed!" << std::endl; }
  else { std::cout << "passed!" << std::endl; }
  std::cout << " *** Test: SetAnatomicalSpace (DICOM) ";
  myValidator.SetAnatomicalSpace("RAS");
  myValidator.SetAnatomicalSpace("dicom");
  if (myValidator.GetAnatomicalSpace() != LPS ) {std::cout << "failed!" << std::endl; }
  else { std::cout << "passed!" << std::endl; }

  // *** GetSpaceDirection ***
  std::cout << "*** Test: GetSpaceDirection ";
  if (msrFrame != myValidator.GetSpaceDirection()) { std::cout << "failed!" << std::endl; }
  else { std::cout << "passed!" << std::endl; }

  // *** SetSpaceDirection ***
  bool is_set_space_direction_passing = true;
  std::cout << "*** Test: SetSpaceDirection ";
  vnl_matrix_fixed<double, 3, 3>     sdir;
  sdir.fill(0.333);
  myValidator.SetSpaceDirection(sdir);
  msrFrame = myValidator.GetSpaceDirection();
  for (unsigned int i=0; i<3; i++)
    {
    for (unsigned int j=0; j<3; j++)
      {
      if (msrFrame[i][j] != 0.333)
        { is_set_space_direction_passing = false; }
      }
    }
  std::cout << "*** Test: SetSpaceDirection ";
  if (is_set_space_direction_passing) { std::cout << "passed!" << std::endl; }
  else { std::cout << "failed!" << std::endl; }
  // TODO: GetSizes after SetKinds
  std::cout << "-------------" << std::endl;

  typedef GradientVectorType::iterator                        MyIter;
  GradientVectorType gradient = myValidator.GetGradient(1);
  for (MyIter i=std::begin(gradient); i != std::end(gradient); i++)
    {
    std::cout << i << " ";
    }
  std::cout << std::endl;

  // Set "units"
  myValidator.SetUnits( 'mm' );

  // Set "space origin"
  myValidator.SetOrigin( nrrdVolume->GetOrigin() );

  // SetSize and SetNumberOfComponents are used to define the 'sizes' field
  myValidator.SetSize( nrrdVolume->GetLargestPossibleRegion().Size() );
  myValidator.SetNumberOfComponents( nrrdVolume->GetVectorLength() );

  // Create sapce direction from the input spacing and input direction
  // Should it be moved to MetaDataDictionaryValidator !!!?
  ImageType::DirectionType inputDirection = nrrdVolume->GetDirection();
  ImageType::SpacingType   inputSpacing = nrrdVolume->GetSpacing();
  ImageType::DirectionType spacingMatrix;
  spacingMatrix.Fill(0.0);
  spacingMatrix[0][0] = inputSpacing[0];
  spacingMatrix[1][1] = inputSpacing[1];
  spacingMatrix[2][2] = inputSpacing[2];
  ImageType::DirectionType spaceDirections = inputDirection * spacingMatrix;
  myValidator.SetSpaceDirection( spaceDirections );

  // Note that inside the validator, "space directions" is created from SetSpaceDirection and SetInterleaved
  // The same fact holds for "thicknesses"
  // Also, there is no direct need for SetCenterings and SetKinds, since they are created from SetInterleaved

  // Set "DWMRI__gradient_xxxx"
  typedef std::array<double,3>       GradientDirType;
  typedef std::vector< GradientDir > GradientDirTableType;
  GradientDirType       myGradient;
  GradientDirTableType  myGradientTable;
  myGradientTable.Reserve( nrrdVolume->GetVectorLength() )
  // #0 -> b0
  myGradient[0] = 0; myGradient[1] = 0; myGradient[2] = 0;
  myGradientTable.push_back( myGradient );
  // #1
  myGradient[0] = 1; myGradient[1] = 0; myGradient[2] = 0;
  myGradientTable.push_back( myGradient );
  // #2
  myGradient[0] = 0; myGradient[1] = 1; myGradient[2] = 0;
  myGradientTable.push_back( myGradient );
  // #3
  myGradient[0] = 0; myGradient[1] = 0; myGradient[2] = 1;
  myGradientTable.push_back( myGradient );
  // #4 -> b0
  myGradient[0] = 0; myGradient[1] = 0; myGradient[2] = 0;
  myGradientTable.push_back( myGradient );
  // #5
  myGradient[0] = 0.5; myGradient[1] = 0; myGradient[2] = 0;
  myGradientTable.push_back( myGradient );
  // #6
  myGradient[0] = 0; myGradient[1] = 0.5; myGradient[2] = 0;
  myGradientTable.push_back( myGradient );
  // #7
  myGradient[0] = 0; myGradient[1] = 0; myGradient[2] = 0.5;
  myGradientTable.push_back( myGradient );

  myValidator.ClearAllGradients();
  myValidator.SetAllGradients( myGradientTable );

  // Add meta data to nrrd volume
  nrrdVolume->SetMetaDataDictionary(myValidator->GetMetaDataDictionary());

  // Write Nrrd volume to disk
  typedef itk::ImageFileWriter<ImageType> WriterType;
  WriterType::Pointer nrrdWriter = WriterType::New();
  nrrdWriter->UseCompressionOn();
  nrrdWriter->UseInputMetaDataDictionaryOn();
  nrrdWriter->SetInput( nrrdVolume );
  nrrdWriter->SetFileName( './testNrrdImage.nrrd' );
  try
    {
    nrrdWriter->Update();
    }
  catch( itk::ExceptionObject & e )
    {
    std::cerr << e << std::endl;
    }
*/
    }
  catch (...)
    {
    // clean up from an exception
    // nrrdNix(nrrd);
    // rethrow exception
    throw;
    }

  std::cout << "SUCCESS!" << std::endl;
  return EXIT_SUCCESS;
}
