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
 */
#include "DWIMetaDataDictionaryValidator.h"

#include <string>
#include <vector>
#include <array>
// #include "itkMetaDataObject.h"
#include "itkMetaDataDictionary.h"

DWIMetaDataDictionaryValidator::DWIMetaDataDictionaryValidator()
{
  this->is_valid = false;
  // this->SetMetaDataDictionary();
}

DWIMetaDataDictionaryValidator::~DWIMetaDataDictionaryValidator() {}

std::string DWIMetaDataDictionaryValidator::GetGradientKeyString(int index)
{
  char               tmpStr[64];
  sprintf(tmpStr, "DWMRI_gradient_%04u", index);
  std::string key(tmpStr);
  return key;
}

void DWIMetaDataDictionaryValidator::SetKey(std::string key, std::string value)
{
  itk::EncapsulateMetaData<std::string>(m_dict, key, value);
  /*try
    {
    itk::EncapsulateMetaData<std::string>(m_dict, key, value);
    }
  catch (...)
    {
    throw;
    }*/
}

DWIMetaDataDictionaryValidator::MetaDataDictionaryType DWIMetaDataDictionaryValidator::GetMetaDataDictionary()
{
  return m_dict;
}

void DWIMetaDataDictionaryValidator::SetMetaDataDictionary(DWIMetaDataDictionaryValidator::ConstMetaDataDictionaryType rhs)
{
  m_dict = rhs;
}

int DWIMetaDataDictionaryValidator::GetNumberOfDimensions()
{
  itkGenericExceptionMacro("NRRD metadata in ITK does not (currently) store the number of dimensions!");
  /*
  int retval;
  if (m_dict.HasKey("dimension"))
    {
    itk::ExposeMetaData<int>(m_dict, "dimension", retval);
    }
  else if (m_dict.HasKey("NRRD_dimension"))
    {
    itk::ExposeMetaData<int>(m_dict, "NRRD_dimension", retval);
    }
  else { itkGenericExceptionMacro("Could not find the number of dimensions!"); }
  return retval;
  */
}

void DWIMetaDataDictionaryValidator::SetNumberOfDimensions(const int rhs)
{
  itkGenericExceptionMacro("NRRD metadata in ITK does not (currently) store the number of dimensions!");
}

void DWIMetaDataDictionaryValidator::SetNumberOfDimensions(const DWIMetaDataDictionaryValidator::Integer3x1ArrayType rhs)
{
  itkGenericExceptionMacro("NRRD metadata in ITK does not (currently) store the number of dimensions!");
  /*
  int dims = rhs.size();
  try
    {
    itk::EncapsulateMetaData<int>(m_dict, "dimension", dims);
    }
  catch (...)
    {
    itkGenericExceptionMacro("Could not set number of dimensions!");
    }
  */
}

DWIMetaDataDictionaryValidator::MeasurementFrameType DWIMetaDataDictionaryValidator::GetMeasurementFrame()
{
  DWIMetaDataDictionaryValidator::MeasurementFrameType retval;
  itk::ExposeMetaData<DWIMetaDataDictionaryValidator::MeasurementFrameType>(m_dict, "NRRD_measurement frame", retval);
  if (retval.size() != 0)
    {
    return retval;
    }
  else
    {
    itkGenericExceptionMacro("Measurement frame not found in metadata");
    }
}

void DWIMetaDataDictionaryValidator::SetMeasurementFrame(DWIMetaDataDictionaryValidator::MeasurementFrameType input)
{
  std::string       key = "NRRD_measurement frame";
  itk::EncapsulateMetaData<DWIMetaDataDictionaryValidator::MeasurementFrameType>(m_dict, key, input);
}

DWIMetaDataDictionaryValidator::Double3x1ArrayType DWIMetaDataDictionaryValidator::GetGradient(int index)
{
     DWIMetaDataDictionaryValidator::Double3x1ArrayType   currentGradient;
     std::string key = DWIMetaDataDictionaryValidator::GetGradientKeyString(index);
     // m_dict.Print(std::cout);
     if (m_dict.HasKey(key))
       {
       std::string   NrrdValue;
       itk::ExposeMetaData<std::string>(m_dict, key, NrrdValue);
       // std::cout << "NrrdValue: " << NrrdValue.c_str() << std::endl;
       sscanf(NrrdValue.c_str(), "%lf %lf %lf", &currentGradient[0], &currentGradient[1], &currentGradient[2]);
       // std::cout << currentGradient[0] << "," << currentGradient[1] << "," << currentGradient[2] << std::endl;
       }
     else
       {
       itkGenericExceptionMacro("Gradient not found in metadata");
       }
     return currentGradient;
   }

void DWIMetaDataDictionaryValidator::SetGradient(int index, DWIMetaDataDictionaryValidator::Double3x1ArrayType & gradient)
{
  std::string       key = DWIMetaDataDictionaryValidator::GetGradientKeyString(index);
  char              tmp[64];
  sprintf(tmp, "%lf %lf %lf", gradient[0], gradient[1], gradient[2]);
  std::string       value(tmp);
  DWIMetaDataDictionaryValidator::SetKey(key, value);
}

int DWIMetaDataDictionaryValidator::GetGradientCount()
   {
     DWIMetaDataDictionaryValidator::StringVectorType keys = m_dict.GetKeys();
     int count = 0;
     for (DWIMetaDataDictionaryValidator::StringVectorType::iterator it = keys.begin(); it != keys.end(); ++it)
       {
       if((*it).find("DWMRI_gradient_") != std::string::npos)
         {
         count++;
         }
       else if((*it).find("DWMRI_NEX") != std::string::npos)
         {
         int repeats;
         itk::ExposeMetaData<int>(m_dict, "DWMRI_NEX", repeats);
         count += repeats;
         }
       }
     return count;
   }

DWIMetaDataDictionaryValidator::GradientTableType DWIMetaDataDictionaryValidator::GetGradientTable()
   {
     // std::cout << "GetGradientTable" << std::endl;
     DWIMetaDataDictionaryValidator::GradientTableType myGradientTable;
     DWIMetaDataDictionaryValidator::StringVectorType allKeys = m_dict.GetKeys();
     // std::cout << allKeys[0] << std::endl;
     int count = 0;
     for (DWIMetaDataDictionaryValidator::StringVectorType::iterator it = allKeys.begin();
          it != allKeys.end();
          ++it)
       {
       if((*it).find("DWMRI_gradient_") != std::string::npos)
         {
         std::string   gradientString = (*it).substr(15,4);
         int index = std::stoi(gradientString);
         DWIMetaDataDictionaryValidator::Double3x1ArrayType curGradientDirection = GetGradient(index);
         myGradientTable.push_back(curGradientDirection);
         }
       count++;
       }
     return myGradientTable;
   }

void DWIMetaDataDictionaryValidator::SetGradientTable(DWIMetaDataDictionaryValidator::GradientTableType & myGradientTable)
     {
     std::stringstream outputGradDirMetaDataStream;
     DWIMetaDataDictionaryValidator::StringVectorType keys = m_dict.GetKeys();
     //for (int i = 0; i < keys.size(); )
     int count = 0;
     while (!myGradientTable.empty())
       {
       DWIMetaDataDictionaryValidator::Double3x1ArrayType currentGradient = myGradientTable.back();
       DWIMetaDataDictionaryValidator::SetGradient(count, currentGradient);
       myGradientTable.pop_back();
       count++;
       }

     std::string nextKey = DWIMetaDataDictionaryValidator::GetGradientKeyString(++count);
     while (m_dict.HasKey(nextKey))
       {
       m_dict.Erase(nextKey);
       std::string nextKey = DWIMetaDataDictionaryValidator::GetGradientKeyString(++count);
       }
     }

void DWIMetaDataDictionaryValidator::DeleteGradientTable()
{
  for (itk::MetaDataDictionary::MetaDataDictionaryMapType::iterator it = m_dict.Begin();
       it != m_dict.End(); )
       {
       //http://stackoverflow.com/questions/8234779/how-to-remove-from-a-map-while-iterating-it
       if( (*it).first.find("DWMRI_gradient_") != std::string::npos )
         {
         m_dict.Erase((*it).first);
         }
       else
         {
         it++;
         }
       }
     }

double DWIMetaDataDictionaryValidator::GetBValue()
{
  double      retval = 0.0;
  std::string valstr;
  itk::ExposeMetaData<std::string>(m_dict, "DWMRI_b-value", valstr);
  std::stringstream ss(valstr);
  ss >> retval;
  return retval;
}

void DWIMetaDataDictionaryValidator::SetBValue(const int bvalue)
{
  double            value(bvalue);
  DWIMetaDataDictionaryValidator::SetBValue(value);  // TODO: Should template this?
}

void DWIMetaDataDictionaryValidator::SetBValue(const double bvalue)
{
  std::string        key = "DWMRI_b-value";
  char               tmpStr[64];
  sprintf(tmpStr, "%f", bvalue);
  std::string        valstr(tmpStr);
  DWIMetaDataDictionaryValidator::SetKey(key, valstr);
}

/*
   bool DWIMetaDataDictionaryValidator::IsValidDWIHeader() const
   {
     return false;
    // TODO:  check that the dictionary has all the required header information for NRRD
    // DWI writing
   }

   //TODO: Move code from gtractDWIResampleInPlace here
*/
