/**
  * \author Hans J. Johnson
  * This class is used to build a meta-data dictionary
  * and to help validate that the meta-data dictionary
  * is suitable to writing valid DWI data to disk
  * in a nrrd file
  *
  * TODO: Determine if extensions may be made to facilitate writing
  * nifti files with associated bvec and bval files
  */
#ifndef DWIMetaDataDictionaryValidator_h_
#define DWIMetaDataDictionaryValidator_h_

#include "itkMetaDataDictionary.h"

#include <vector>
#include <array>

typedef std::array<double,3> GradientDirType;
typedef std::vector< GradientDir > GradientDirTableType;

/* Psuedocode for how this Validator could be used

DWIMetaDataDictionaryValidator myValidator; // It's empty
GradientDirType      myG;
GradientDirTableType myGT;
myGT.reserve(80);

myG[0] =1; myG[1] =0; myG[2] = 0;
myGT.push_back( myG );
myG[0] =0; myG[1] =1; myG[2] = 0;
myGT.push_back( myG );
myG[0] =0; myG[1] =0; myG[2] = 1;
myGT.push_back( myG );
myG[0] =0; myG[1] =0; myG[2] = 0;
myGT.push_back( myG );

myValidator.ClearAllGradients();
myValidator.SetAllGradients( myGT );

myValidator.SetBValue(1000);
myValidator.GetBValue();
myValidator.SetMeasurementFrame( matrix & );
myValidator.SetKinds();
myValidator.SetUnits();

*/

class DWIMetaDataDictionaryValidator
{
 public:
   DWIMetaDataDictionaryValidator() {};
   ~DWIMetaDataDictionaryValidator() {};
   DWIMetaDataDictionaryValidator & operator=(const DWIMetaDataDictionaryValidator & rhs)
   {
      m_dict = rhs.m_dict;
   }

   GradientDirTableType ExtractGradientTableValues() const
   {
     GradientDirTableType myGradientTable;
     std::stringstream outputGradDirMetaDataStream;
     //TODO: COMPLETE THIS HACK
     for ( MetaDataDictionaryMapType::iterator it = m_dict->begin();
       it != m_dict->end();
       it++ )
       {
       //http://stackoverflow.com/questions/8234779/how-to-remove-from-a-map-while-iterating-it
       size_t start_pos = 
       if( (*it).first.find("DWMRI_gradient_") != std::npos )
         {
         // Get Current Gradient Direction
         GradientDirType curGradientDirection;
         char               tmpStr[64];
         sprintf(tmpStr, "DWMRI_gradient_%04u", i);
         std::string KeyString(tmpStr);
         std::string NrrdValue;

         itk::ExposeMetaData<std::string>(m_dict, KeyString, NrrdValue);
         sscanf(
           NrrdValue.c_str(), " %lf %lf %lf", &curGradientDirection[0], &curGradientDirection[1], &curGradientDirection[2]);
         }
       }
     return myGradientTable;
   }

   void AddAllGradients(const GradientDirTableType & myGradientTable)
     {
     std::stringstream outputGradDirMetaDataStream;
     //TODO:  Replace code 
     // Rotate gradient vectors by rigid transform and inverse measurement frame
     for( unsigned int i = 0; i < myGradientTable.size(); ++i )

     }

   void ClearAllGradients()
     {
     for ( MetaDataDictionaryMapType::iterator it = m_dict->begin();
       it != m_dict->end();
       /* nothinghere */ )
       {
       //http://stackoverflow.com/questions/8234779/how-to-remove-from-a-map-while-iterating-it
       if( (*it).first.find("DWMRI_gradient_") != std::npos )
         {
         m_Dictionary.erase(it++);
         }
       else
         {
         it++;
         }
       }
     }

   bool IsValidDWIHeader() const
   {
    // TODO:  check that the dictionary has all the required header information for NRRD
    // DWI writing
   }

   //TODO: Move code from gtractDWIResampleInPlace here
   
 
 private:
   itk::itkMetaDataDictionary m_dict;

};
#endif // DWIMetaDataDictionaryValidator_h_
