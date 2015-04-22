/**
 * \author Ali Ghayoor
 * This test file creates an ITK vector image,
 * Creates a NRRD header file for that using DWIMetaDataDictionaryValidator,
 * and writes the image to the disk in NRRD format.
 */

#include <itkImage.h>
#include <itkVectorImage.h>
#include <itkImageFileWriter.h>

#include "DWIMetaDataDictionaryValidator.h"

typedef short                           PixelValue;
typedef itk::VectorImage<PixelValue, 3> ImageType;

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
     if( i==0 || i=4 )
       {
       variableLengthVector[i] = 0; // assumed as b0 images
       }
     else
       {
       variableLengthVector[i] = i*10; // assumed as 6 gradient components
       }
     }

  nrrdVolume.FillBuffer(variableLengthVector);
  return nrrdVolume;
}

int main(int argc, char * argv[])
{
  // Create a vector image
  ImageType::Pointer nrrdVolume = CreateVolume();

  // Create NRRD header file using DWIMetaDataDictionaryValidator
  DWIMetaDataDictionaryValidator myValidator;
  myValidator.Initialize();

  // Set "type"
  myValidator.SetType( 'short' );

  // Set "endian"
  myValidator.SetEndian( 'little' );

  // Set "encoding"
  myValidator.SetEncoding( 'raw' );

  // Set "units"
  myValidator.SetUnits( 'mm' );

  // Set "space"
  myValidator.SetAnatomicalSpace( 'LPS' );

  /*
  From the SetInterleaved you should be able to make the following fields:
  - kinds
      "kinds: space space space list": volume interleaved
      "kinds: space space list space": slice interleaved
      "kinds: list space space space": pixel interleaved
  - centerings
  Also, it affects the formate of:
  - space directions: where 'none' is placed
  - thicknesses: order on 'NaN's
  - sizes: where numebr of components is placed
  */
  myValidator.SetInterleaved( 'pixel' );

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

  // Set "measurement frame"
  itk::Matrix<double,3,3> measurementFrame;
  measurementFrame.SetIdentity();
  myValidator.SetMeasurementFrame( measurementFrame );

  // Set "DWMRI_b-value"
  myValidator.SetBValue(1000); // It is an integer

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

  return EXIT_SUCCESS;
}
