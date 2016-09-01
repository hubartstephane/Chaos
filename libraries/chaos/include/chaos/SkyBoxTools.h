#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/ImageTools.h>

namespace chaos
{

/**
 * SkyBoxSingleDisposition : the disposition of each individual face inside the single image
 */

class SkyBoxSingleDisposition
{
public:

  /** the disposition for an horizontal image */
  static SkyBoxSingleDisposition const HorizontalDisposition;
  /** the disposition for an vertical image */
  static SkyBoxSingleDisposition const VerticalDisposition;

  /** returns the position according to an image type */
  glm::ivec3 GetPositionAndFlags(int image_type) const;

  /** the number of aligned images horizontally */
  int image_count_horiz;
  /** the number of aligned images vertically */
  int image_count_vert;
  /** corner for left image + flags */
  glm::ivec3 left_image_position;
  /** corner for right image + flags */
  glm::ivec3 right_image_position;
  /** corner for top image + flags */
  glm::ivec3 top_image_position;
  /** corner for bottom image + flags */
  glm::ivec3 bottom_image_position;
  /** corner for front image + flags */
  glm::ivec3 front_image_position;
  /** corner for back image + flags */
  glm::ivec3 back_image_position;
};

class SkyBoxImages
{
public:

  static int const IMAGE_LEFT   = 0;
  static int const IMAGE_RIGHT  = 1;
  static int const IMAGE_TOP    = 2;
  static int const IMAGE_BOTTOM = 3;
  static int const IMAGE_FRONT  = 4;
  static int const IMAGE_BACK   = 5;
  static int const IMAGE_SINGLE = 6;

  static int const IMAGE_FIRST_INDEX = IMAGE_LEFT;
  static int const IMAGE_LAST_INDEX  = IMAGE_SINGLE;

  static int const IMAGE_NO_TRANSFORM    = 0;
  static int const IMAGE_CENTRAL_SYMETRY = 1;

  /** constructor */
  SkyBoxImages();
  /** move constructor */
  SkyBoxImages(SkyBoxImages && other) = default;
  /** destructor */
  ~SkyBoxImages();

  /** Get skybox size */
  int GetSkyBoxSize() const;
  /** Get skybox bpp */
  int GetSkyBoxBPP() const;
  /** Get skybox size for a single image (returns -1 if not a good candidate i.e 4/3) */
  static int GetSingleImageSize(FIBITMAP * image);
  /** Get skybox size for a multiple image (returns -1 if not a good candidate i.e square) */
  static int GetMultipleImageSize(FIBITMAP * image);

  /** Method to clean the skybox */
  void Release(bool bFreeMemory = true);
  /** Transform a 6 images box into a one image box */
  SkyBoxImages ToMultipleImages() const;
  /** Transform a single image box into a six images box */
  SkyBoxImages ToSingleImage(bool bHorizontal, glm::vec4 const & fill_color) const;

  /** returns true whether the skybox is empty */
  bool IsEmpty() const;
  /** returns true if the skybox is contained into a single image */
  bool IsSingleImage() const;
  /** returns true if the skybox is contained into six images */
  bool IsMultipleImage() const;
  /** returns true whether the 6 images skybox is completed */
  bool IsMultipleImageComplete() const;
  /** returns true whether it is a single image in horizontal direction */
  bool IsSingleImageHorizontal() const;
  /** returns true whether it is a single image in vertical direction */
  bool IsSingleImageVertical() const;

  /** Gets an image */
  FIBITMAP * GetImage(int image_type);
  /** Gets an image */
  FIBITMAP const * GetImage(int image_type) const;

  /** Gets the description of one face */
  ImageDescription GetImageFaceDescription(int image_type) const;
  /** Gets position and flags for a single image */
  glm::ivec3 GetPositionAndFlags(int image_type) const;

  /** Sets an image (verify that it is a coherent call) */
  bool SetImage(int image_type, FIBITMAP * image, bool bReleasePrevious);
  /** compare 2 images and ensure they are compatible */
  static bool AreImageCompatible(FIBITMAP * image1, FIBITMAP * image2);

protected:

  /** simply set one of the multiple image pointer (no check)*/
  void SetImagePtrUnchecked(int image_type, FIBITMAP * image);

protected:

  /** whenever the skybox is contained into a single image */
  FIBITMAP * single_image;
  /** left image */
  FIBITMAP * left_image;
  /** right image */
  FIBITMAP * right_image;
  /** top image */
  FIBITMAP * top_image;
  /** bottom image */
  FIBITMAP * bottom_image;
  /** front image */
  FIBITMAP * front_image;
  /** back image */
  FIBITMAP * back_image;
};



/** 
 * SkyBoxTools : deserve to load skybox files
 */

class SkyBoxTools
{
public:

  /** load a skybox from a single file */
  static SkyBoxImages LoadSingleSkyBox(char const * filename);
  /** load a skybox from a multiple files */
  static SkyBoxImages LoadMultipleSkyBox(char const * left_image, char const * right_image, char const * top_image, char const * bottom_image, char const * front_image, char const * back_image);


protected:

  /** utility function to load a file into one image. Incrementaly test for compatibility with previsous image */
  static bool DoLoadMultipleSkyBox_OneImage(SkyBoxImages & skybox, char const * filename, int image_index);
};

}; // namespace chaos

