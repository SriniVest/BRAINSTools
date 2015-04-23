/*=========================================================================
 *
 *  Copyright SINAPSE: Scalable Informatics for Neuroscience, Processing
 *                     and Software Engineering
 *            The University of Iowa
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *         http://www.apache.org/licenses/LICENSE-2.0.txt
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
*=========================================================================*/
/*
 *  \author Hans J. Johnson, David M. Welch
 *
 *
 * This class is used to build a meta-data dictionary and validate that the
 * meta-data dictionary is suitable to writing valid DWI data to disk in nrrd
 * file format.
 *
 * TODO: Determine if extensions may be made to facilitate writing
 * nifti files with associated bvec and bval files
 *
 */

#ifndef DWIMetaDataDictionaryValidator_h_
#define DWIMetaDataDictionaryValidator_h_

#include <string>
#include <vector>
#include <array>
#include "NrrdIO.h"
#include "itkMetaDataObject.h"
#include "itkMetaDataDictionary.h"


class DWIMetaDataDictionaryValidator
{
 private:
  itk::MetaDataDictionary m_dict;
  bool is_valid;
  void SetKey(std::string, std::string);

 protected:
  std::string GetGradientKeyString(int);

 public:
  // 3D
  typedef std::array<int, 3>                   Integer3x1ArrayType;
  typedef std::array<double, 3>                Double3x1ArrayType;
  typedef std::array<std::string, 3>           String3x1ArrayType;
  // 4D
  typedef std::array<int, 4>                   Integer4x1ArrayType;
  typedef std::array<double, 4>                Double4ArrayType;
  typedef std::array<std::string, 4>           String4x1ArrayType;

  typedef std::vector<double>                  DoubleVectorType;
  typedef std::vector<std::string>             StringVectorType;
  typedef std::vector<Double3x1ArrayType >     GradientTableType;
  typedef std::vector<DoubleVectorType>        MeasurementFrameType;
  typedef std::array<DoubleVectorType, 3>      SpaceDirectionType;

  typedef itk::MetaDataDictionary &            MetaDataDictionaryType;
  typedef const itk::MetaDataDictionary &      ConstMetaDataDictionaryType;

  DWIMetaDataDictionaryValidator();
  ~DWIMetaDataDictionaryValidator();
  // metadata dictionary methods
  MetaDataDictionaryType GetMetaDataDictionary();
  void SetMetaDataDictionary( ConstMetaDataDictionaryType );
  // dimension
  // Currently, these just raise an error saying you should use the itk::Image to get this info
  int GetNumberOfDimensions();
  void SetNumberOfDimensions(int);
  void SetNumberOfDimensions(const Integer3x1ArrayType);
  // measurement frame
  MeasurementFrameType GetMeasurementFrame();
  void SetMeasurementFrame(const MeasurementFrameType );
  // gradients
  Double3x1ArrayType GetGradient(int);
  int GetGradientCount();
  void SetGradient(int, Double3x1ArrayType & );
  GradientTableType GetGradientTable();
  void SetGradientTable(GradientTableType & );
  void DeleteGradientTable();
  // b-value
  double GetBValue();
  void SetBValue(const int);
  void SetBValue(const double);
  /*
  // sizes
  Integer3x1ArrayType GetSizes();
  void SetSizes(const Integer3x1ArrayType);
  // centering
  StringVectorType GetCenterings();
  void SetCentering(const int, const std::string);
  void SetCentering(const int, const int);  // use NrrdIO.h enum
  void SetCenterings(const Integer3x1ArrayType);
  void SetCenterings(const String3x1ArrayType);
  // kinds
  int GetKind(const int);
  Integer4x1ArrayType GetKinds();
  String4x1ArrayType GetKinds();
  void SetNonSpatialAxis(const int);
  void RearrangeDimensions(const Integer4x1ArrayType);  // [2, 1, 0, 3]
  // type
  int GetType();
  std::string GetType();
  void SetType(const std::string);
  void SetType(const int);
  // endian
  int GetEndian();
  std::string GetEndian();
  void SetEndian(const std::string);
  void SetEndian(const int);
  // encoding
  int GetEncoding();
  std::string GetEncoding();
  void SetEncoding(const std::string);
  void SetEncoding(const int);
  // anatomical space
  AnatomicalSpaceEnum GetAnatomicalSpace();
  std::string  GetAnatomicalSpace();
  void SetAnatomicalSpace(int );
  void SetAnatomicalSpace(std::string );
  // direction
  SpaceDirectionType GetSpaceDirection();
  void SetSpaceDirection(SpaceDirectionType);
  // units
  String3x1ArrayType GetSpaceUnits();
  void GetSpaceOrigin(String3x1ArrayType );
  // origin
  Double3x1ArrayType GetSpaceOrigin();
  void SetSpaceOrigin(Double3x1ArrayType);

  // conveinence functions
  bool IsValidDWIHeader();

  void Update();
 protected:
  enum AnatomicalSpaceEnum { Unknown, RAS, LPS };
  // void PrintSelf(std::stream &os, Indent indent) const ITK_OVERRIDE;
  */

};

#endif // DWIMetaDataDictionaryValidator_h_
