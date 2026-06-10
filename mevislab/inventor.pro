#
#  QMake build description for Open Inventor on Mac OS X, Windows, Linux
#
#  author: Felix Ritter
#

TEMPLATE = lib

TARGET = Inventor

# build a framework bundle on Mac OS X
macx::CONFIG += lib_bundle

CONFIG += dll lex yacc exceptions shared thread ivgensources
CONFIG += opengl libFL jpeg iconv glew

# Inventor uses fontconfig on Linux
unix:!macx:CONFIG += fontconfig

DESTDIR    = ../../../lib
DLLDESTDIR = ../../../lib

win32 {
  QMAKE_YACC = $(MEVIS_THIRDPARTY_LIB)/Sources/inventor/Inventor/build/windows/yacc.exe
  QMAKE_CXXFLAGS += /TP

  DEFINES += WIN32 INVENTOR_EXPORTS __SGIOIV_BUILD_DLL
  DEFINES -= UNICODE
  LIBS += Ws2_32.lib shell32.lib
}
else:macx {
  # see also end of file for mac framework config
  LIBS += -framework Carbon
}
else {
  # Inventor uses dlopen on Linux
  LIBS += -ldl
}

debug::DEFINES += DEBUG

include( $(MLAB_ROOT)/MeVis/Foundation/Configuration/SystemInit.pri )

linux {
  64bit:DEFINES += __ia64__
}

# libFL
libFL {
  INCLUDEPATH += \
    libFL/ang \
    libFL/src

  QMAKE_LIBDIR += libFL
  win32::LIBS += FL$${d}.lib
   unix::LIBS += -lFL$${d}

  CONFIG += freetype
}

# include settings of required libraries
include( $(MEVIS_THIRDPARTY_LIB)/Configuration/MeVis_ThirdParty.pri )

linux:TARGET = $${LIB_NAME_PREFIX}$${TARGET}

INCLUDEPATH += \
    lib/database/include \
    lib/interaction/include \
    lib/nodekits/include \
    lib/mevis/include

# required for SoCalcParse.y
INCLUDEPATH += \
    lib/database/src/so/engines

# nurbs
INCLUDEPATH += \
    lib/database/src/so/nodes \
    lib/database/src/so/nodes/nurbs \
    lib/database/src/so/nodes/nurbs/head
DEFINES += LIBRARYBUILD

#
#  MeVis extensions to Open Inventor
#

# REMEMBER TO ADD HEADER FILES THAT SHOULD BE PART OF THE PUBLIC INVENTOR API 
# TO THE MACOSX FRAMEWORK SECTIONS FURTHER DOWN !!!
    
win32::HEADERS += \
    lib/mevis/include/SoWinEnterScope.h \
    lib/mevis/include/SoWinLeaveScope.h \
    lib/mevis/src/utf8_filesupport_win32.h

HEADERS += \
    lib/mevis/include/Inventor/SoCatch.h \
    lib/mevis/include/Inventor/SoInventor.h \
    lib/mevis/include/Inventor/SoMeVis.h \
    lib/mevis/include/Inventor/SoTrace.h \
    lib/mevis/include/Inventor/SoProfiling.h \
    lib/mevis/include/Inventor/elements/SoGLPolygonOffsetElement.h \
    lib/mevis/include/Inventor/elements/SoGLStencilBitsElement.h \
    lib/mevis/include/Inventor/elements/SoPolygonOffsetElement.h \
    lib/mevis/include/Inventor/elements/SoStencilBitsElement.h \
    lib/mevis/include/Inventor/elements/SoGLVBOElement.h \
    lib/mevis/include/Inventor/events/SoMouseWheelEvent.h \
    lib/mevis/include/Inventor/events/SoLocation2RefreshEvent.h \
    lib/mevis/include/Inventor/fields/SoSFVec2s.h \
    lib/mevis/include/Inventor/fields/SoSFVec3s.h \
    lib/mevis/include/Inventor/nodes/SoPolygonOffset.h \
    lib/mevis/include/Inventor/system/SbConfig.h \
    lib/mevis/include/Inventor/system/SbSystem.h \
    lib/mevis/include/Inventor/system/SbOpenGL.h \
    lib/mevis/include/Inventor/system/SbOpenGLU.h \
    lib/mevis/include/Inventor/misc/SoVBO.h \
    lib/mevis/include/Inventor/misc/SoVertexArrayIndexer.h

SOURCES += \
    lib/mevis/src/SoCatch.cpp \
    lib/mevis/src/SoMeVis.cpp \
    lib/mevis/src/SoTrace.cpp \
    lib/mevis/src/SoProfiling.cpp \
    lib/mevis/src/elements/SoGLPolygonOffsetElement.cpp \
    lib/mevis/src/elements/SoGLStencilBitsElement.cpp \
    lib/mevis/src/elements/SoPolygonOffsetElement.cpp \
    lib/mevis/src/elements/SoStencilBitsElement.cpp \
    lib/mevis/src/elements/SoGLVBOElement.cpp \
    lib/mevis/src/events/SoMouseWheelEvent.cpp \
    lib/mevis/src/events/SoLocation2RefreshEvent.cpp \
    lib/mevis/src/fields/SoSFVec2s.cpp \
    lib/mevis/src/fields/SoSFVec3s.cpp \
    lib/mevis/src/nodes/SoPolygonOffset.cpp \
    lib/mevis/src/system/SbOpenGL.cpp \
    lib/mevis/src/misc/SoVBO.cpp \
    lib/mevis/src/misc/SoVertexArrayIndexer.cpp


win32::SOURCES += \
    lib/mevis/src/utf8_filesupport_win32.cpp


#
#  SGI Open Inventor 2.1.x
#

HEADERS += \
    lib/database/include/Inventor/Sb.h \
    lib/database/include/Inventor/SbBasic.h \
    lib/database/include/Inventor/SbBox.h \
    lib/database/include/Inventor/SbBoxd.h \
    lib/database/include/Inventor/SbColor.h \
    lib/database/include/Inventor/SbDict.h \
    lib/database/include/Inventor/SbLinear.h \
    lib/database/include/Inventor/SbLineard.h \
    lib/database/include/Inventor/SbPList.h \
    lib/database/include/Inventor/SbString.h \
    lib/database/include/Inventor/SbTime.h \
    lib/database/include/Inventor/SbTypeDefs.h \
    lib/database/include/Inventor/SbViewportRegion.h \
    lib/database/include/Inventor/So.h \
    lib/database/include/Inventor/SoDB.h \
    lib/database/include/Inventor/SoInput.h \
    lib/database/include/Inventor/SoLists.h \
    lib/database/include/Inventor/SoOutput.h \
    lib/database/include/Inventor/SoPath.h \
    lib/database/include/Inventor/SoPickedPoint.h \
    lib/database/include/Inventor/SoPrimitiveVertex.h \
    lib/database/include/Inventor/SoType.h \
    lib/database/include/Inventor/actions/SoAction.h \
    lib/database/include/Inventor/actions/SoActions.h \
    lib/database/include/Inventor/actions/SoCallbackAction.h \
    lib/database/include/Inventor/actions/SoGLRenderAction.h \
    lib/database/include/Inventor/actions/SoGetBoundingBoxAction.h \
    lib/database/include/Inventor/actions/SoGetMatrixAction.h \
    lib/database/include/Inventor/actions/SoHandleEventAction.h \
    lib/database/include/Inventor/actions/SoPickAction.h \
    lib/database/include/Inventor/actions/SoRayPickAction.h \
    lib/database/include/Inventor/actions/SoSearchAction.h \
    lib/database/include/Inventor/actions/SoSubAction.h \
    lib/database/include/Inventor/actions/SoWriteAction.h \
    lib/database/include/Inventor/bundles/SoBundle.h \
    lib/database/include/Inventor/bundles/SoMaterialBundle.h \
    lib/database/include/Inventor/bundles/SoNormalBundle.h \
    lib/database/include/Inventor/bundles/SoTextureCoordinateBundle.h \
    lib/database/include/Inventor/caches/SoBoundingBoxCache.h \
    lib/database/include/Inventor/caches/SoCache.h \
    lib/database/include/Inventor/caches/SoGLCacheList.h \
    lib/database/include/Inventor/caches/SoGLRenderCache.h \
    lib/database/include/Inventor/caches/SoNormalCache.h \
    lib/database/include/Inventor/details/SoConeDetail.h \
    lib/database/include/Inventor/details/SoCubeDetail.h \
    lib/database/include/Inventor/details/SoCylinderDetail.h \
    lib/database/include/Inventor/details/SoDetail.h \
    lib/database/include/Inventor/details/SoDetails.h \
    lib/database/include/Inventor/details/SoFaceDetail.h \
    lib/database/include/Inventor/details/SoLineDetail.h \
    lib/database/include/Inventor/details/SoPointDetail.h \
    lib/database/include/Inventor/details/SoSubDetail.h \
    lib/database/include/Inventor/details/SoTextDetail.h \
    lib/database/include/Inventor/elements/SoAccumulatedElement.h \
    lib/database/include/Inventor/elements/SoAmbientColorElement.h \
    lib/database/include/Inventor/elements/SoBBoxModelMatrixElement.h \
    lib/database/include/Inventor/elements/SoCacheElement.h \
    lib/database/include/Inventor/elements/SoClipPlaneElement.h \
    lib/database/include/Inventor/elements/SoComplexityElement.h \
    lib/database/include/Inventor/elements/SoComplexityTypeElement.h \
    lib/database/include/Inventor/elements/SoCoordinateElement.h \
    lib/database/include/Inventor/elements/SoCreaseAngleElement.h \
    lib/database/include/Inventor/elements/SoCullVolumeElement.h \
    lib/database/include/Inventor/elements/SoCurrentGLMaterialElement.h \
    lib/database/include/Inventor/elements/SoDiffuseColorElement.h \
    lib/database/include/Inventor/elements/SoDrawStyleElement.h \
    lib/database/include/Inventor/elements/SoElement.h \
    lib/database/include/Inventor/elements/SoElements.h \
    lib/database/include/Inventor/elements/SoEmissiveColorElement.h \
    lib/database/include/Inventor/elements/SoFloatElement.h \
    lib/database/include/Inventor/elements/SoFocalDistanceElement.h \
    lib/database/include/Inventor/elements/SoFontNameElement.h \
    lib/database/include/Inventor/elements/SoFontSizeElement.h \
    lib/database/include/Inventor/elements/SoGLAmbientColorElement.h \
    lib/database/include/Inventor/elements/SoGLCacheContextElement.h \
    lib/database/include/Inventor/elements/SoGLClipPlaneElement.h \
    lib/database/include/Inventor/elements/SoGLColorIndexElement.h \
    lib/database/include/Inventor/elements/SoGLCoordinateElement.h \
    lib/database/include/Inventor/elements/SoGLDiffuseColorElement.h \
    lib/database/include/Inventor/elements/SoGLDrawStyleElement.h \
    lib/database/include/Inventor/elements/SoGLEmissiveColorElement.h \
    lib/database/include/Inventor/elements/SoGLLazyElement.h \
    lib/database/include/Inventor/elements/SoGLLightIdElement.h \
    lib/database/include/Inventor/elements/SoGLLightModelElement.h \
    lib/database/include/Inventor/elements/SoGLLinePatternElement.h \
    lib/database/include/Inventor/elements/SoGLLineWidthElement.h \
    lib/database/include/Inventor/elements/SoGLModelMatrixElement.h \
    lib/database/include/Inventor/elements/SoGLNormalElement.h \
    lib/database/include/Inventor/elements/SoGLPointSizeElement.h \
    lib/database/include/Inventor/elements/SoGLPolygonStippleElement.h \
    lib/database/include/Inventor/elements/SoGLProjectionMatrixElement.h \
    lib/database/include/Inventor/elements/SoGLRenderPassElement.h \
    lib/database/include/Inventor/elements/SoGLShapeHintsElement.h \
    lib/database/include/Inventor/elements/SoGLShininessElement.h \
    lib/database/include/Inventor/elements/SoGLSpecularColorElement.h \
    lib/database/include/Inventor/elements/SoGLTextureBlendColorElement.h \
    lib/database/include/Inventor/elements/SoGLTextureCoordinateElement.h \
    lib/database/include/Inventor/elements/SoGLTextureEnabledElement.h \
    lib/database/include/Inventor/elements/SoGLTextureImageElement.h \
    lib/database/include/Inventor/elements/SoGLTextureMatrixElement.h \
    lib/database/include/Inventor/elements/SoGLTextureModelElement.h \
    lib/database/include/Inventor/elements/SoGLTextureQualityElement.h \
    lib/database/include/Inventor/elements/SoGLTextureWrapSElement.h \
    lib/database/include/Inventor/elements/SoGLTextureWrapTElement.h \
    lib/database/include/Inventor/elements/SoGLUpdateAreaElement.h \
    lib/database/include/Inventor/elements/SoGLViewingMatrixElement.h \
    lib/database/include/Inventor/elements/SoGLViewportRegionElement.h \
    lib/database/include/Inventor/elements/SoInt32Element.h \
    lib/database/include/Inventor/elements/SoLazyElement.h \
    lib/database/include/Inventor/elements/SoLightAttenuationElement.h \
    lib/database/include/Inventor/elements/SoLightModelElement.h \
    lib/database/include/Inventor/elements/SoLinePatternElement.h \
    lib/database/include/Inventor/elements/SoLineWidthElement.h \
    lib/database/include/Inventor/elements/SoLocalBBoxMatrixElement.h \
    lib/database/include/Inventor/elements/SoLongElement.h \
    lib/database/include/Inventor/elements/SoMaterialBindingElement.h \
    lib/database/include/Inventor/elements/SoModelMatrixElement.h \
    lib/database/include/Inventor/elements/SoNormalBindingElement.h \
    lib/database/include/Inventor/elements/SoNormalElement.h \
    lib/database/include/Inventor/elements/SoOverrideElement.h \
    lib/database/include/Inventor/elements/SoPickRayElement.h \
    lib/database/include/Inventor/elements/SoPickStyleElement.h \
    lib/database/include/Inventor/elements/SoPointSizeElement.h \
    lib/database/include/Inventor/elements/SoProfileCoordinateElement.h \
    lib/database/include/Inventor/elements/SoProfileElement.h \
    lib/database/include/Inventor/elements/SoProjectionMatrixElement.h \
    lib/database/include/Inventor/elements/SoReplacedElement.h \
    lib/database/include/Inventor/elements/SoShapeHintsElement.h \
    lib/database/include/Inventor/elements/SoShapeStyleElement.h \
    lib/database/include/Inventor/elements/SoShininessElement.h \
    lib/database/include/Inventor/elements/SoSpecularColorElement.h \
    lib/database/include/Inventor/elements/SoSubElement.h \
    lib/database/include/Inventor/elements/SoSwitchElement.h \
    lib/database/include/Inventor/elements/SoTextureBlendColorElement.h \
    lib/database/include/Inventor/elements/SoTextureCoordinateBindingElement.h \
    lib/database/include/Inventor/elements/SoTextureCoordinateElement.h \
    lib/database/include/Inventor/elements/SoTextureImageElement.h \
    lib/database/include/Inventor/elements/SoTextureMatrixElement.h \
    lib/database/include/Inventor/elements/SoTextureModelElement.h \
    lib/database/include/Inventor/elements/SoTextureQualityElement.h \
    lib/database/include/Inventor/elements/SoTextureWrapSElement.h \
    lib/database/include/Inventor/elements/SoTextureWrapTElement.h \
    lib/database/include/Inventor/elements/SoTransparencyElement.h \
    lib/database/include/Inventor/elements/SoUnitsElement.h \
    lib/database/include/Inventor/elements/SoViewVolumeElement.h \
    lib/database/include/Inventor/elements/SoViewingMatrixElement.h \
    lib/database/include/Inventor/elements/SoViewportRegionElement.h \
    lib/database/include/Inventor/elements/SoWindowElement.h \
    lib/database/include/Inventor/engines/SoBoolOperation.h \
    lib/database/include/Inventor/engines/SoCalculator.h \
    lib/database/include/Inventor/engines/SoCompose.h \
    lib/database/include/Inventor/engines/SoComputeBoundingBox.h \
    lib/database/include/Inventor/engines/SoConcatenate.h \
    lib/database/include/Inventor/engines/SoCounter.h \
    lib/database/include/Inventor/engines/SoElapsedTime.h \
    lib/database/include/Inventor/engines/SoEngine.h \
    lib/database/include/Inventor/engines/SoEngines.h \
    lib/database/include/Inventor/engines/SoFieldConverter.h \
    lib/database/include/Inventor/engines/SoGate.h \
    lib/database/include/Inventor/engines/SoInterpolate.h \
    lib/database/include/Inventor/engines/SoOnOff.h \
    lib/database/include/Inventor/engines/SoOneShot.h \
    lib/database/include/Inventor/engines/SoOutputData.h \
    lib/database/include/Inventor/engines/SoSelectOne.h \
    lib/database/include/Inventor/engines/SoSubEngine.h \
    lib/database/include/Inventor/engines/SoTimeCounter.h \
    lib/database/include/Inventor/engines/SoTransformVec3f.h \
    lib/database/include/Inventor/engines/SoTriggerAny.h \
    lib/database/include/Inventor/errors/SoDebugError.h \
    lib/database/include/Inventor/errors/SoError.h \
    lib/database/include/Inventor/errors/SoErrors.h \
    lib/database/include/Inventor/errors/SoMemoryError.h \
    lib/database/include/Inventor/errors/SoReadError.h \
    lib/database/include/Inventor/events/SoButtonEvent.h \
    lib/database/include/Inventor/events/SoEvent.h \
    lib/database/include/Inventor/events/SoEvents.h \
    lib/database/include/Inventor/events/SoKeyboardEvent.h \
    lib/database/include/Inventor/events/SoLocation2Event.h \
    lib/database/include/Inventor/events/SoMotion3Event.h \
    lib/database/include/Inventor/events/SoMouseButtonEvent.h \
    lib/database/include/Inventor/events/SoSpaceballButtonEvent.h \
    lib/database/include/Inventor/events/SoSubEvent.h \
    lib/database/include/Inventor/fields/SoField.h \
    lib/database/include/Inventor/fields/SoFieldContainer.h \
    lib/database/include/Inventor/fields/SoFieldData.h \
    lib/database/include/Inventor/fields/SoFields.h \
    lib/database/include/Inventor/fields/SoMFBitMask.h \
    lib/database/include/Inventor/fields/SoMFBool.h \
    lib/database/include/Inventor/fields/SoMFColor.h \
    lib/database/include/Inventor/fields/SoMFDouble.h \
    lib/database/include/Inventor/fields/SoMFEngine.h \
    lib/database/include/Inventor/fields/SoMFEnum.h \
    lib/database/include/Inventor/fields/SoMFFloat.h \
    lib/database/include/Inventor/fields/SoMFInt32.h \
    lib/database/include/Inventor/fields/SoMFLong.h \
    lib/database/include/Inventor/fields/SoMFMatrix.h \
    lib/database/include/Inventor/fields/SoMFMatrixd.h \
    lib/database/include/Inventor/fields/SoMFName.h \
    lib/database/include/Inventor/fields/SoMFNode.h \
    lib/database/include/Inventor/fields/SoMFPath.h \
    lib/database/include/Inventor/fields/SoMFPlane.h \
    lib/database/include/Inventor/fields/SoMFPlaned.h \
    lib/database/include/Inventor/fields/SoMFRotation.h \
    lib/database/include/Inventor/fields/SoMFRotationd.h \
    lib/database/include/Inventor/fields/SoMFShort.h \
    lib/database/include/Inventor/fields/SoMFString.h \
    lib/database/include/Inventor/fields/SoMFTime.h \
    lib/database/include/Inventor/fields/SoMFUInt32.h \
    lib/database/include/Inventor/fields/SoMFULong.h \
    lib/database/include/Inventor/fields/SoMFUShort.h \
    lib/database/include/Inventor/fields/SoMFVec2f.h \
    lib/database/include/Inventor/fields/SoMFVec2d.h \
    lib/database/include/Inventor/fields/SoMFVec3f.h \
    lib/database/include/Inventor/fields/SoMFVec3d.h \
    lib/database/include/Inventor/fields/SoMFVec4f.h \
    lib/database/include/Inventor/fields/SoMFVec4d.h \
    lib/database/include/Inventor/fields/SoSFBitMask.h \
    lib/database/include/Inventor/fields/SoSFBool.h \
    lib/database/include/Inventor/fields/SoSFColor.h \
    lib/database/include/Inventor/fields/SoSFDouble.h \
    lib/database/include/Inventor/fields/SoSFEngine.h \
    lib/database/include/Inventor/fields/SoSFEnum.h \
    lib/database/include/Inventor/fields/SoSFFloat.h \
    lib/database/include/Inventor/fields/SoSFImage.h \
    lib/database/include/Inventor/fields/SoSFInt32.h \
    lib/database/include/Inventor/fields/SoSFLong.h \
    lib/database/include/Inventor/fields/SoSFMatrix.h \
    lib/database/include/Inventor/fields/SoSFMatrixd.h \
    lib/database/include/Inventor/fields/SoSFName.h \
    lib/database/include/Inventor/fields/SoSFNode.h \
    lib/database/include/Inventor/fields/SoSFPath.h \
    lib/database/include/Inventor/fields/SoSFPlane.h \
    lib/database/include/Inventor/fields/SoSFPlaned.h \
    lib/database/include/Inventor/fields/SoSFRotation.h \
    lib/database/include/Inventor/fields/SoSFRotationd.h \
    lib/database/include/Inventor/fields/SoSFShort.h \
    lib/database/include/Inventor/fields/SoSFString.h \
    lib/database/include/Inventor/fields/SoSFTime.h \
    lib/database/include/Inventor/fields/SoSFTrigger.h \
    lib/database/include/Inventor/fields/SoSFUInt32.h \
    lib/database/include/Inventor/fields/SoSFULong.h \
    lib/database/include/Inventor/fields/SoSFUShort.h \
    lib/database/include/Inventor/fields/SoSFVec2f.h \
    lib/database/include/Inventor/fields/SoSFVec2d.h \
    lib/database/include/Inventor/fields/SoSFVec3f.h \
    lib/database/include/Inventor/fields/SoSFVec3d.h \
    lib/database/include/Inventor/fields/SoSFVec4f.h \
    lib/database/include/Inventor/fields/SoSFVec4d.h \
    lib/database/include/Inventor/fields/SoSubField.h \
    lib/database/include/Inventor/misc/SoAuditorList.h \
    lib/database/include/Inventor/misc/SoBase.h \
    lib/database/include/Inventor/misc/SoBasic.h \
    lib/database/include/Inventor/misc/SoByteStream.h \
    lib/database/include/Inventor/misc/SoCallbackList.h \
    lib/database/include/Inventor/misc/SoChildList.h \
    lib/database/include/Inventor/misc/SoCompactPathList.h \
    lib/database/include/Inventor/misc/SoNormalGenerator.h \
    lib/database/include/Inventor/misc/SoNotification.h \
    lib/database/include/Inventor/misc/SoState.h \
    lib/database/include/Inventor/misc/SoTempPath.h \
    lib/database/include/Inventor/misc/SoTranscribe.h \
    lib/database/include/Inventor/misc/upgraders/SoUpgrader.h \
    lib/database/include/Inventor/misc/upgraders/SoV1CustomNode.h \
    lib/database/include/Inventor/nodes/SoAnnotation.h \
    lib/database/include/Inventor/nodes/SoArray.h \
    lib/database/include/Inventor/nodes/SoAsciiText.h \
    lib/database/include/Inventor/nodes/SoBaseColor.h \
    lib/database/include/Inventor/nodes/SoBlinker.h \
    lib/database/include/Inventor/nodes/SoCallback.h \
    lib/database/include/Inventor/nodes/SoCamera.h \
    lib/database/include/Inventor/nodes/SoClipPlane.h \
    lib/database/include/Inventor/nodes/SoColorIndex.h \
    lib/database/include/Inventor/nodes/SoComplexity.h \
    lib/database/include/Inventor/nodes/SoCone.h \
    lib/database/include/Inventor/nodes/SoCoordinate3.h \
    lib/database/include/Inventor/nodes/SoCoordinate4.h \
    lib/database/include/Inventor/nodes/SoCube.h \
    lib/database/include/Inventor/nodes/SoCylinder.h \
    lib/database/include/Inventor/nodes/SoDirectionalLight.h \
    lib/database/include/Inventor/nodes/SoDrawStyle.h \
    lib/database/include/Inventor/nodes/SoEnvironment.h \
    lib/database/include/Inventor/nodes/SoEventCallback.h \
    lib/database/include/Inventor/nodes/SoFaceSet.h \
    lib/database/include/Inventor/nodes/SoFile.h \
    lib/database/include/Inventor/nodes/SoFont.h \
    lib/database/include/Inventor/nodes/SoFontStyle.h \
    lib/database/include/Inventor/nodes/SoGroup.h \
    lib/database/include/Inventor/nodes/SoIndexedFaceSet.h \
    lib/database/include/Inventor/nodes/SoIndexedTriangleSet.h \
    lib/database/include/Inventor/nodes/SoIndexedLineSet.h \
    lib/database/include/Inventor/nodes/SoIndexedNurbsCurve.h \
    lib/database/include/Inventor/nodes/SoIndexedNurbsSurface.h \
    lib/database/include/Inventor/nodes/SoIndexedShape.h \
    lib/database/include/Inventor/nodes/SoIndexedTriangleStripSet.h \
    lib/database/include/Inventor/nodes/SoInfo.h \
    lib/database/include/Inventor/nodes/SoLOD.h \
    lib/database/include/Inventor/nodes/SoLabel.h \
    lib/database/include/Inventor/nodes/SoLevelOfDetail.h \
    lib/database/include/Inventor/nodes/SoLight.h \
    lib/database/include/Inventor/nodes/SoLightModel.h \
    lib/database/include/Inventor/nodes/SoLineSet.h \
    lib/database/include/Inventor/nodes/SoLinearProfile.h \
    lib/database/include/Inventor/nodes/SoLocateHighlight.h \
    lib/database/include/Inventor/nodes/SoMaterial.h \
    lib/database/include/Inventor/nodes/SoMaterialBinding.h \
    lib/database/include/Inventor/nodes/SoMaterialIndex.h \
    lib/database/include/Inventor/nodes/SoMatrixTransform.h \
    lib/database/include/Inventor/nodes/SoMultipleCopy.h \
    lib/database/include/Inventor/nodes/SoNode.h \
    lib/database/include/Inventor/nodes/SoNodes.h \
    lib/database/include/Inventor/nodes/SoNonIndexedShape.h \
    lib/database/include/Inventor/nodes/SoNormal.h \
    lib/database/include/Inventor/nodes/SoNormalBinding.h \
    lib/database/include/Inventor/nodes/SoNurbsCurve.h \
    lib/database/include/Inventor/nodes/SoNurbsProfile.h \
    lib/database/include/Inventor/nodes/SoNurbsSurface.h \
    lib/database/include/Inventor/nodes/SoOrthographicCamera.h \
    lib/database/include/Inventor/nodes/SoPackedColor.h \
    lib/database/include/Inventor/nodes/SoPathSwitch.h \
    lib/database/include/Inventor/nodes/SoPendulum.h \
    lib/database/include/Inventor/nodes/SoPerspectiveCamera.h \
    lib/database/include/Inventor/nodes/SoPickStyle.h \
    lib/database/include/Inventor/nodes/SoPointLight.h \
    lib/database/include/Inventor/nodes/SoPointSet.h \
    lib/database/include/Inventor/nodes/SoProfile.h \
    lib/database/include/Inventor/nodes/SoProfileCoordinate2.h \
    lib/database/include/Inventor/nodes/SoProfileCoordinate3.h \
    lib/database/include/Inventor/nodes/SoQuadMesh.h \
    lib/database/include/Inventor/nodes/SoResetTransform.h \
    lib/database/include/Inventor/nodes/SoRotation.h \
    lib/database/include/Inventor/nodes/SoRotationXYZ.h \
    lib/database/include/Inventor/nodes/SoRotor.h \
    lib/database/include/Inventor/nodes/SoScale.h \
    lib/database/include/Inventor/nodes/SoSeparator.h \
    lib/database/include/Inventor/nodes/SoShape.h \
    lib/database/include/Inventor/nodes/SoShapeHints.h \
    lib/database/include/Inventor/nodes/SoShuttle.h \
    lib/database/include/Inventor/nodes/SoSphere.h \
    lib/database/include/Inventor/nodes/SoSpotLight.h \
    lib/database/include/Inventor/nodes/SoSubNode.h \
    lib/database/include/Inventor/nodes/SoSwitch.h \
    lib/database/include/Inventor/nodes/SoText2.h \
    lib/database/include/Inventor/nodes/SoText3.h \
    lib/database/include/Inventor/nodes/SoTexture2.h \
    lib/database/include/Inventor/nodes/SoTexture2Transform.h \
    lib/database/include/Inventor/nodes/SoTextureCoordinate2.h \
    lib/database/include/Inventor/nodes/SoTextureCoordinateBinding.h \
    lib/database/include/Inventor/nodes/SoTextureCoordinateDefault.h \
    lib/database/include/Inventor/nodes/SoTextureCoordinateEnvironment.h \
    lib/database/include/Inventor/nodes/SoTextureCoordinateFunction.h \
    lib/database/include/Inventor/nodes/SoTextureCoordinatePlane.h \
    lib/database/include/Inventor/nodes/SoTransform.h \
    lib/database/include/Inventor/nodes/SoTransformSeparator.h \
    lib/database/include/Inventor/nodes/SoTransformation.h \
    lib/database/include/Inventor/nodes/SoTranslation.h \
    lib/database/include/Inventor/nodes/SoTriangleStripSet.h \
    lib/database/include/Inventor/nodes/SoUnits.h \
    lib/database/include/Inventor/nodes/SoVertexProperty.h \
    lib/database/include/Inventor/nodes/SoVertexShape.h \
    lib/database/include/Inventor/nodes/SoWWWAnchor.h \
    lib/database/include/Inventor/nodes/SoWWWInline.h \
    lib/database/include/Inventor/projectors/SbCylinderPlaneProjector.h \
    lib/database/include/Inventor/projectors/SbCylinderProjector.h \
    lib/database/include/Inventor/projectors/SbCylinderSectionProjector.h \
    lib/database/include/Inventor/projectors/SbCylinderSheetProjector.h \
    lib/database/include/Inventor/projectors/SbLineProjector.h \
    lib/database/include/Inventor/projectors/SbPlaneProjector.h \
    lib/database/include/Inventor/projectors/SbProjector.h \
    lib/database/include/Inventor/projectors/SbProjectors.h \
    lib/database/include/Inventor/projectors/SbSpherePlaneProjector.h \
    lib/database/include/Inventor/projectors/SbSphereProjector.h \
    lib/database/include/Inventor/projectors/SbSphereSectionProjector.h \
    lib/database/include/Inventor/projectors/SbSphereSheetProjector.h \
    lib/database/include/Inventor/sensors/SoAlarmSensor.h \
    lib/database/include/Inventor/sensors/SoDataSensor.h \
    lib/database/include/Inventor/sensors/SoDelayQueueSensor.h \
    lib/database/include/Inventor/sensors/SoFieldSensor.h \
    lib/database/include/Inventor/sensors/SoIdleSensor.h \
    lib/database/include/Inventor/sensors/SoNodeSensor.h \
    lib/database/include/Inventor/sensors/SoOneShotSensor.h \
    lib/database/include/Inventor/sensors/SoPathSensor.h \
    lib/database/include/Inventor/sensors/SoSensor.h \
    lib/database/include/Inventor/sensors/SoSensorManager.h \
    lib/database/include/Inventor/sensors/SoSensors.h \
    lib/database/include/Inventor/sensors/SoTimerQueueSensor.h \
    lib/database/include/Inventor/sensors/SoTimerSensor.h \
    lib/database/include/Inventor/SoDebug.h \
    lib/database/include/Inventor/SoMachine.h \
    lib/database/include/SoUnknownEngine.h \
    lib/database/include/SoUnknownNode.h \
    lib/database/src/so/engines/SoCalcExpr.h \
    lib/database/src/so/engines/SoEngineUtil.h \
    lib/database/src/so/engines/SoFieldConverters.h \
    lib/database/src/so/fields/SoGlobalField.h \
    lib/database/src/so/nodes/nurbs/SoAddPrefix.h \
    lib/database/src/so/nodes/nurbs/SoCurveMaps.h \
    lib/database/src/so/nodes/nurbs/SoCurveRender.h \
    lib/database/src/so/nodes/nurbs/SoCurveRenderEval.h \
    lib/database/src/so/nodes/nurbs/SoGLRender.h \
    lib/database/src/so/nodes/nurbs/SoPickEvals.h \
    lib/database/src/so/nodes/nurbs/SoPickMaps.h \
    lib/database/src/so/nodes/nurbs/SoPickRender.h \
    lib/database/src/so/nodes/nurbs/SoPrimEvals.h \
    lib/database/src/so/nodes/nurbs/SoPrimMaps.h \
    lib/database/src/so/nodes/nurbs/SoPrimRender.h \
    lib/database/src/so/nodes/nurbs/_defines.h \
    lib/database/src/so/nodes/nurbs/_undefs.h \
    lib/database/src/so/nodes/nurbs/clients/gl4base.h \
    lib/database/src/so/nodes/nurbs/clients/gl4curveval.h \
    lib/database/src/so/nodes/nurbs/clients/gl4surfeval.h \
    lib/database/src/so/nodes/nurbs/clients/gl4types.h \
    lib/database/src/so/nodes/nurbs/clients/isocurveeval.h \
    lib/database/src/so/nodes/nurbs/clients/softcurvemaps.h \
    lib/database/src/so/nodes/nurbs/clients/softcurveval.h \
    lib/database/src/so/nodes/nurbs/clients/softcurvmap.h \
    lib/database/src/so/nodes/nurbs/clients/softrenderer.h \
    lib/database/src/so/nodes/nurbs/clients/softsurfacemaps.h \
    lib/database/src/so/nodes/nurbs/clients/softsurfeval.h \
    lib/database/src/so/nodes/nurbs/clients/softsurfmap.h \
    lib/database/src/so/nodes/nurbs/clients/typestringtable.h \
    lib/database/src/so/nodes/nurbs/head/arc.h \
    lib/database/src/so/nodes/nurbs/head/arcsorter.h \
    lib/database/src/so/nodes/nurbs/head/arctess.h \
    lib/database/src/so/nodes/nurbs/head/backend.h \
    lib/database/src/so/nodes/nurbs/head/basiccrveval.h \
    lib/database/src/so/nodes/nurbs/head/basicsurfeval.h \
    lib/database/src/so/nodes/nurbs/head/bezierarc.h \
    lib/database/src/so/nodes/nurbs/head/bin.h \
    lib/database/src/so/nodes/nurbs/head/bufpool.h \
    lib/database/src/so/nodes/nurbs/head/cachingeval.h \
    lib/database/src/so/nodes/nurbs/head/coveandtiler.h \
    lib/database/src/so/nodes/nurbs/head/curve.h \
    lib/database/src/so/nodes/nurbs/head/curvelist.h \
    lib/database/src/so/nodes/nurbs/head/defines.h \
    lib/database/src/so/nodes/nurbs/head/displaylist.h \
    lib/database/src/so/nodes/nurbs/head/displaymode.h \
    lib/database/src/so/nodes/nurbs/head/flist.h \
    lib/database/src/so/nodes/nurbs/head/flistsorter.h \
    lib/database/src/so/nodes/nurbs/head/glimports.h \
    lib/database/src/so/nodes/nurbs/head/gridline.h \
    lib/database/src/so/nodes/nurbs/head/gridtrimvertex.h \
    lib/database/src/so/nodes/nurbs/head/gridvertex.h \
    lib/database/src/so/nodes/nurbs/head/hull.h \
    lib/database/src/so/nodes/nurbs/head/jarcloc.h \
    lib/database/src/so/nodes/nurbs/head/jumpbuffer.h \
    lib/database/src/so/nodes/nurbs/head/knot.h \
    lib/database/src/so/nodes/nurbs/head/knotvector.h \
    lib/database/src/so/nodes/nurbs/head/mapdesc.h \
    lib/database/src/so/nodes/nurbs/head/maplist.h \
    lib/database/src/so/nodes/nurbs/head/mesher.h \
    lib/database/src/so/nodes/nurbs/head/myassert.h \
    lib/database/src/so/nodes/nurbs/head/myglimports.h \
    lib/database/src/so/nodes/nurbs/head/mymath.h \
    lib/database/src/so/nodes/nurbs/head/mysetjmp.h \
    lib/database/src/so/nodes/nurbs/head/mystdio.h \
    lib/database/src/so/nodes/nurbs/head/mystdlib.h \
    lib/database/src/so/nodes/nurbs/head/mystring.h \
    lib/database/src/so/nodes/nurbs/head/nurbsconsts.h \
    lib/database/src/so/nodes/nurbs/head/nurbstess.h \
    lib/database/src/so/nodes/nurbs/head/patch.h \
    lib/database/src/so/nodes/nurbs/head/patchlist.h \
    lib/database/src/so/nodes/nurbs/head/pwlarc.h \
    lib/database/src/so/nodes/nurbs/head/quilt.h \
    lib/database/src/so/nodes/nurbs/head/reader.h \
    lib/database/src/so/nodes/nurbs/head/renderhints.h \
    lib/database/src/so/nodes/nurbs/head/simplemath.h \
    lib/database/src/so/nodes/nurbs/head/slicer.h \
    lib/database/src/so/nodes/nurbs/head/sorter.h \
    lib/database/src/so/nodes/nurbs/head/subdivider.h \
    lib/database/src/so/nodes/nurbs/head/trimline.h \
    lib/database/src/so/nodes/nurbs/head/trimregion.h \
    lib/database/src/so/nodes/nurbs/head/trimvertex.h \
    lib/database/src/so/nodes/nurbs/head/trimvertpool.h \
    lib/database/src/so/nodes/nurbs/head/types.h \
    lib/database/src/so/nodes/nurbs/head/uarray.h \
    lib/database/src/so/nodes/nurbs/head/varray.h \
    lib/database/src/so/nodes/readGIF.h \
    lib/database/src/so/nodes/readJPEG.h \
    lib/database/src/so/upgraders/SoUpgraders.h \
    lib/database/src/so/upgraders/SoV1DrawStyle.h \
    lib/database/src/so/upgraders/SoV1Environment.h \
    lib/database/src/so/upgraders/SoV1IndexedTriangleMesh.h \
    lib/database/src/so/upgraders/SoV1LayerGroup.h \
    lib/database/src/so/upgraders/SoV1LightModel.h \
    lib/database/src/so/upgraders/SoV1Material.h \
    lib/database/src/so/upgraders/SoV1PackedColor.h \
    lib/database/src/so/upgraders/SoV1PickStyle.h \
    lib/database/src/so/upgraders/SoV1Separator.h \
    lib/database/src/so/upgraders/SoV1ShapeHints.h \
    lib/database/src/so/upgraders/SoV1Text2.h \
    lib/database/src/so/upgraders/SoV1Text3.h \
    lib/database/src/so/upgraders/SoV1Texture2.h \
    lib/database/src/so/upgraders/SoV1TextureCoordinateCube.h \
    lib/database/src/so/upgraders/SoV1TextureCoordinateCylinder.h \
    lib/database/src/so/upgraders/SoV1TextureCoordinateEnvironment.h \
    lib/database/src/so/upgraders/SoV1TextureCoordinatePlane.h \
    lib/database/src/so/upgraders/SoV1TextureCoordinateSphere.h \
    lib/database/src/so/upgraders/SoV2AsciiText.h \
    lib/database/src/so/upgraders/SoV2FontStyle.h \
    lib/database/src/so/upgraders/SoV2LOD.h \
    lib/database/src/so/upgraders/SoV2MaterialIndex.h \
    lib/database/src/so/upgraders/SoV2Text2.h \
    lib/database/src/so/upgraders/SoV2Text3.h \
    lib/database/src/so/upgraders/SoV2VertexProperty.h \
    lib/database/src/so/upgraders/SoV2WWWAnchor.h \
    lib/database/src/so/upgraders/SoV2WWWInline.h \
    lib/interaction/include/Inventor/SoInteraction.h \
    lib/interaction/include/Inventor/SoSceneManager.h \
    lib/interaction/include/Inventor/actions/SoBoxHighlightRenderAction.h \
    lib/interaction/include/Inventor/actions/SoLineHighlightRenderAction.h \
    lib/interaction/include/Inventor/draggers/SoCenterballDragger.h \
    lib/interaction/include/Inventor/draggers/SoDirectionalLightDragger.h \
    lib/interaction/include/Inventor/draggers/SoDragPointDragger.h \
    lib/interaction/include/Inventor/draggers/SoDragger.h \
    lib/interaction/include/Inventor/draggers/SoHandleBoxDragger.h \
    lib/interaction/include/Inventor/draggers/SoJackDragger.h \
    lib/interaction/include/Inventor/draggers/SoPointLightDragger.h \
    lib/interaction/include/Inventor/draggers/SoRotateCylindricalDragger.h \
    lib/interaction/include/Inventor/draggers/SoRotateDiscDragger.h \
    lib/interaction/include/Inventor/draggers/SoRotateSphericalDragger.h \
    lib/interaction/include/Inventor/draggers/SoScale1Dragger.h \
    lib/interaction/include/Inventor/draggers/SoScale2Dragger.h \
    lib/interaction/include/Inventor/draggers/SoScale2UniformDragger.h \
    lib/interaction/include/Inventor/draggers/SoScaleUniformDragger.h \
    lib/interaction/include/Inventor/draggers/SoSpotLightDragger.h \
    lib/interaction/include/Inventor/draggers/SoTabBoxDragger.h \
    lib/interaction/include/Inventor/draggers/SoTabPlaneDragger.h \
    lib/interaction/include/Inventor/draggers/SoTrackballDragger.h \
    lib/interaction/include/Inventor/draggers/SoTransformBoxDragger.h \
    lib/interaction/include/Inventor/draggers/SoTransformerDragger.h \
    lib/interaction/include/Inventor/draggers/SoTranslate1Dragger.h \
    lib/interaction/include/Inventor/draggers/SoTranslate2Dragger.h \
    lib/interaction/include/Inventor/manips/SoCenterballManip.h \
    lib/interaction/include/Inventor/manips/SoDirectionalLightManip.h \
    lib/interaction/include/Inventor/manips/SoHandleBoxManip.h \
    lib/interaction/include/Inventor/manips/SoJackManip.h \
    lib/interaction/include/Inventor/manips/SoPointLightManip.h \
    lib/interaction/include/Inventor/manips/SoSpotLightManip.h \
    lib/interaction/include/Inventor/manips/SoTabBoxManip.h \
    lib/interaction/include/Inventor/manips/SoTrackballManip.h \
    lib/interaction/include/Inventor/manips/SoTransformBoxManip.h \
    lib/interaction/include/Inventor/manips/SoTransformManip.h \
    lib/interaction/include/Inventor/manips/SoTransformerManip.h \
    lib/interaction/include/Inventor/nodekits/SoInteractionKit.h \
    lib/interaction/include/Inventor/nodes/SoAntiSquish.h \
    lib/interaction/include/Inventor/nodes/SoSelection.h \
    lib/interaction/include/Inventor/nodes/SoSurroundScale.h \
    lib/interaction/src/draggers/geom/SoCenterballDraggerGeom.h \
    lib/interaction/src/draggers/geom/SoDirectionalLightDraggerGeom.h \
    lib/interaction/src/draggers/geom/SoDragPointDraggerGeom.h \
    lib/interaction/src/draggers/geom/SoHandleBoxDraggerGeom.h \
    lib/interaction/src/draggers/geom/SoJackDraggerGeom.h \
    lib/interaction/src/draggers/geom/SoPointLightDraggerGeom.h \
    lib/interaction/src/draggers/geom/SoRotateCylindricalDraggerGeom.h \
    lib/interaction/src/draggers/geom/SoRotateDiscDraggerGeom.h \
    lib/interaction/src/draggers/geom/SoRotateSphericalDraggerGeom.h \
    lib/interaction/src/draggers/geom/SoScale1DraggerGeom.h \
    lib/interaction/src/draggers/geom/SoScale2DraggerGeom.h \
    lib/interaction/src/draggers/geom/SoScale2UniformDraggerGeom.h \
    lib/interaction/src/draggers/geom/SoScaleUniformDraggerGeom.h \
    lib/interaction/src/draggers/geom/SoSpotLightDraggerGeom.h \
    lib/interaction/src/draggers/geom/SoTabBoxDraggerGeom.h \
    lib/interaction/src/draggers/geom/SoTabPlaneDraggerGeom.h \
    lib/interaction/src/draggers/geom/SoTrackballDraggerGeom.h \
    lib/interaction/src/draggers/geom/SoTransformBoxDraggerGeom.h \
    lib/interaction/src/draggers/geom/SoTransformerDraggerGeom.h \
    lib/interaction/src/draggers/geom/SoTranslate1DraggerGeom.h \
    lib/interaction/src/draggers/geom/SoTranslate2DraggerGeom.h \
    lib/nodekits/include/Inventor/SoNodeKitPath.h \
    lib/nodekits/include/Inventor/details/SoNodeKitDetail.h \
    lib/nodekits/include/Inventor/misc/upgraders/SoV1AppearanceKit.h \
    lib/nodekits/include/Inventor/misc/upgraders/SoV1BaseKit.h \
    lib/nodekits/include/Inventor/misc/upgraders/SoV1CameraKit.h \
    lib/nodekits/include/Inventor/misc/upgraders/SoV1ConeKit.h \
    lib/nodekits/include/Inventor/misc/upgraders/SoV1CubeKit.h \
    lib/nodekits/include/Inventor/misc/upgraders/SoV1CylinderKit.h \
    lib/nodekits/include/Inventor/misc/upgraders/SoV1DirectionalLightKit.h \
    lib/nodekits/include/Inventor/misc/upgraders/SoV1FaceSetKit.h \
    lib/nodekits/include/Inventor/misc/upgraders/SoV1GroupKit.h \
    lib/nodekits/include/Inventor/misc/upgraders/SoV1IndexedFaceSetKit.h \
    lib/nodekits/include/Inventor/misc/upgraders/SoV1IndexedLineSetKit.h \
    lib/nodekits/include/Inventor/misc/upgraders/SoV1IndexedNurbsCurveKit.h \
    lib/nodekits/include/Inventor/misc/upgraders/SoV1IndexedNurbsSurfaceKit.h \
    lib/nodekits/include/Inventor/misc/upgraders/SoV1IndexedTriangleMeshKit.h \
    lib/nodekits/include/Inventor/misc/upgraders/SoV1LightKit.h \
    lib/nodekits/include/Inventor/misc/upgraders/SoV1LineSetKit.h \
    lib/nodekits/include/Inventor/misc/upgraders/SoV1NodeKit.h \
    lib/nodekits/include/Inventor/misc/upgraders/SoV1NodekitCatalog.h \
    lib/nodekits/include/Inventor/misc/upgraders/SoV1NodekitParts.h \
    lib/nodekits/include/Inventor/misc/upgraders/SoV1NurbsCurveKit.h \
    lib/nodekits/include/Inventor/misc/upgraders/SoV1NurbsSurfaceKit.h \
    lib/nodekits/include/Inventor/misc/upgraders/SoV1OrthographicCameraKit.h \
    lib/nodekits/include/Inventor/misc/upgraders/SoV1PerspectiveCameraKit.h \
    lib/nodekits/include/Inventor/misc/upgraders/SoV1PointLightKit.h \
    lib/nodekits/include/Inventor/misc/upgraders/SoV1PointSetKit.h \
    lib/nodekits/include/Inventor/misc/upgraders/SoV1QuadMeshKit.h \
    lib/nodekits/include/Inventor/misc/upgraders/SoV1SceneKit.h \
    lib/nodekits/include/Inventor/misc/upgraders/SoV1ShapeKit.h \
    lib/nodekits/include/Inventor/misc/upgraders/SoV1SphereKit.h \
    lib/nodekits/include/Inventor/misc/upgraders/SoV1SpotLightKit.h \
    lib/nodekits/include/Inventor/misc/upgraders/SoV1SubKit.h \
    lib/nodekits/include/Inventor/misc/upgraders/SoV1Text2Kit.h \
    lib/nodekits/include/Inventor/misc/upgraders/SoV1Text3Kit.h \
    lib/nodekits/include/Inventor/misc/upgraders/SoV1TriangleStripSetKit.h \
    lib/nodekits/include/Inventor/misc/upgraders/SoV1VertexShapeKit.h \
    lib/nodekits/include/Inventor/misc/upgraders/SoV1WrapperKit.h \
    lib/nodekits/include/Inventor/nodekits/SoAppearanceKit.h \
    lib/nodekits/include/Inventor/nodekits/SoBaseKit.h \
    lib/nodekits/include/Inventor/nodekits/SoCameraKit.h \
    lib/nodekits/include/Inventor/nodekits/SoLightKit.h \
    lib/nodekits/include/Inventor/nodekits/SoNodeKit.h \
    lib/nodekits/include/Inventor/nodekits/SoNodeKitListPart.h \
    lib/nodekits/include/Inventor/nodekits/SoNodekitCatalog.h \
    lib/nodekits/include/Inventor/nodekits/SoNodekitParts.h \
    lib/nodekits/include/Inventor/nodekits/SoSceneKit.h \
    lib/nodekits/include/Inventor/nodekits/SoSeparatorKit.h \
    lib/nodekits/include/Inventor/nodekits/SoShapeKit.h \
    lib/nodekits/include/Inventor/nodekits/SoSubKit.h \
    lib/nodekits/include/Inventor/nodekits/SoWrapperKit.h

PPSOURCES += \
    lib/database/src/so/nodes/SoCone.pp \
    lib/database/src/so/nodes/SoCube.pp \
    lib/database/src/so/nodes/SoCylinder.pp \
    lib/database/src/so/nodes/SoFaceSet.pp \
    lib/database/src/so/nodes/SoIndexedFaceSet.pp \
    lib/database/src/so/nodes/SoIndexedTriangleSet.pp \
    lib/database/src/so/nodes/SoIndexedLineSet.pp \
    lib/database/src/so/nodes/SoIndexedTriangleStripSet.pp \
    lib/database/src/so/nodes/SoLineSet.pp \
    lib/database/src/so/nodes/SoQuadMesh.pp \
    lib/database/src/so/nodes/SoSphere.pp \
    lib/database/src/so/nodes/SoTriangleStripSet.pp

YACCSOURCES = \
    lib/database/src/so/engines/SoCalcParse.y

SOURCES += \
    lib/database/src/sb/SbBox.cpp \
    lib/database/src/sb/SbBoxd.cpp \
    lib/database/src/sb/SbColor.cpp \
    lib/database/src/sb/SbCylinder.cpp \
    lib/database/src/sb/SbCylinderd.cpp \
    lib/database/src/sb/SbDict.cpp \
    lib/database/src/sb/SbLine.cpp \
    lib/database/src/sb/SbLined.cpp \
    lib/database/src/sb/SbMatrix.cpp \
    lib/database/src/sb/SbMatrixd.cpp \
    lib/database/src/sb/SbName.cpp \
    lib/database/src/sb/SbPList.cpp \
    lib/database/src/sb/SbPlane.cpp \
    lib/database/src/sb/SbPlaned.cpp \
    lib/database/src/sb/SbRotation.cpp \
    lib/database/src/sb/SbRotationd.cpp \
    lib/database/src/sb/SbSphere.cpp \
    lib/database/src/sb/SbSphered.cpp \
    lib/database/src/sb/SbString.cpp \
    lib/database/src/sb/SbTime.cpp \
    lib/database/src/sb/SbVec.cpp \
    lib/database/src/sb/SbVecd.cpp \
    lib/database/src/sb/SbView.cpp \
    lib/database/src/sb/SbViewportRegion.cpp \
    lib/database/src/sb/projectors/SbCylinderPlaneProjector.cpp \
    lib/database/src/sb/projectors/SbCylinderProjector.cpp \
    lib/database/src/sb/projectors/SbCylinderSectionProjector.cpp \
    lib/database/src/sb/projectors/SbCylinderSheetProjector.cpp \
    lib/database/src/sb/projectors/SbLineProjector.cpp \
    lib/database/src/sb/projectors/SbPlaneProjector.cpp \
    lib/database/src/sb/projectors/SbProjector.cpp \
    lib/database/src/sb/projectors/SbSpherePlaneProjector.cpp \
    lib/database/src/sb/projectors/SbSphereProjector.cpp \
    lib/database/src/sb/projectors/SbSphereSectionProjector.cpp \
    lib/database/src/sb/projectors/SbSphereSheetProjector.cpp \
    lib/database/src/so/SoAuditorList.cpp \
    lib/database/src/so/SoBase.cpp \
    lib/database/src/so/SoByteStream.cpp \
    lib/database/src/so/SoCBList.cpp \
    lib/database/src/so/SoChildList.cpp \
    lib/database/src/so/SoCompactPathList.cpp \
    lib/database/src/so/SoDB.cpp \
    lib/database/src/so/SoDebug.cpp \
    lib/database/src/so/SoInput.cpp \
    lib/database/src/so/SoLists.cpp \
    lib/database/src/so/SoNormalGenerator.cpp \
    lib/database/src/so/SoNotification.cpp \
    lib/database/src/so/SoOutput.cpp \
    lib/database/src/so/SoPath.cpp \
    lib/database/src/so/SoPickedPoint.cpp \
    lib/database/src/so/SoPrimitiveVertex.cpp \
    lib/database/src/so/SoState.cpp \
    lib/database/src/so/SoTempPath.cpp \
    lib/database/src/so/SoTranscribe.cpp \
    lib/database/src/so/SoType.cpp \
    lib/database/src/so/actions/SoAction.cpp \
    lib/database/src/so/actions/SoActionInit.cpp \
    lib/database/src/so/actions/SoCallbackAction.cpp \
    lib/database/src/so/actions/SoGLRenderAction.cpp \
    lib/database/src/so/actions/SoGetBoundingBoxAction.cpp \
    lib/database/src/so/actions/SoGetMatrixAction.cpp \
    lib/database/src/so/actions/SoHandleEventAction.cpp \
    lib/database/src/so/actions/SoPickAction.cpp \
    lib/database/src/so/actions/SoRayPickAction.cpp \
    lib/database/src/so/actions/SoSearchAction.cpp \
    lib/database/src/so/actions/SoWriteAction.cpp \
    lib/database/src/so/bundles/SoMaterialBundle.cpp \
    lib/database/src/so/bundles/SoNormalBundle.cpp \
    lib/database/src/so/bundles/SoTextureCoordinateBundle.cpp \
    lib/database/src/so/caches/SoBoundingBoxCache.cpp \
    lib/database/src/so/caches/SoCache.cpp \
    lib/database/src/so/caches/SoGLCacheList.cpp \
    lib/database/src/so/caches/SoGLRenderCache.cpp \
    lib/database/src/so/caches/SoNormalCache.cpp \
    lib/database/src/so/details/SoConeDetail.cpp \
    lib/database/src/so/details/SoCubeDetail.cpp \
    lib/database/src/so/details/SoCylinderDetail.cpp \
    lib/database/src/so/details/SoDetail.cpp \
    lib/database/src/so/details/SoDetailInit.cpp \
    lib/database/src/so/details/SoFaceDetail.cpp \
    lib/database/src/so/details/SoLineDetail.cpp \
    lib/database/src/so/details/SoPointDetail.cpp \
    lib/database/src/so/details/SoTextDetail.cpp \
    lib/database/src/so/elements/SoAccumulatedElement.cpp \
    lib/database/src/so/elements/SoAmbientColorElement.cpp \
    lib/database/src/so/elements/SoBBoxModelMatrixElement.cpp \
    lib/database/src/so/elements/SoCacheElement.cpp \
    lib/database/src/so/elements/SoClipPlaneElement.cpp \
    lib/database/src/so/elements/SoComplexityElement.cpp \
    lib/database/src/so/elements/SoComplexityTypeElement.cpp \
    lib/database/src/so/elements/SoCoordinateElement.cpp \
    lib/database/src/so/elements/SoCreaseAngleElement.cpp \
    lib/database/src/so/elements/SoDiffuseColorElement.cpp \
    lib/database/src/so/elements/SoDrawStyleElement.cpp \
    lib/database/src/so/elements/SoElement.cpp \
    lib/database/src/so/elements/SoElementInit.cpp \
    lib/database/src/so/elements/SoEmissiveColorElement.cpp \
    lib/database/src/so/elements/SoFloatElement.cpp \
    lib/database/src/so/elements/SoFocalDistanceElement.cpp \
    lib/database/src/so/elements/SoFontNameElement.cpp \
    lib/database/src/so/elements/SoFontSizeElement.cpp \
    lib/database/src/so/elements/SoGLAmbientColorElement.cpp \
    lib/database/src/so/elements/SoGLCacheContextElement.cpp \
    lib/database/src/so/elements/SoGLClipPlaneElement.cpp \
    lib/database/src/so/elements/SoGLCoordinateElement.cpp \
    lib/database/src/so/elements/SoGLDiffuseColorElement.cpp \
    lib/database/src/so/elements/SoGLDrawStyleElement.cpp \
    lib/database/src/so/elements/SoGLEmissiveColorElement.cpp \
    lib/database/src/so/elements/SoGLLazyElement.cpp \
    lib/database/src/so/elements/SoGLLightIdElement.cpp \
    lib/database/src/so/elements/SoGLLinePatternElement.cpp \
    lib/database/src/so/elements/SoGLLineWidthElement.cpp \
    lib/database/src/so/elements/SoGLModelMatrixElement.cpp \
    lib/database/src/so/elements/SoGLNormalElement.cpp \
    lib/database/src/so/elements/SoGLPointSizeElement.cpp \
    lib/database/src/so/elements/SoGLProjectionMatrixElement.cpp \
    lib/database/src/so/elements/SoGLRenderPassElement.cpp \
    lib/database/src/so/elements/SoGLShapeHintsElement.cpp \
    lib/database/src/so/elements/SoGLShininessElement.cpp \
    lib/database/src/so/elements/SoGLSpecularColorElement.cpp \
    lib/database/src/so/elements/SoGLTextureCoordinateElement.cpp \
    lib/database/src/so/elements/SoGLTextureEnabledElement.cpp \
    lib/database/src/so/elements/SoGLTextureImageElement.cpp \
    lib/database/src/so/elements/SoGLTextureMatrixElement.cpp \
    lib/database/src/so/elements/SoGLUpdateAreaElement.cpp \
    lib/database/src/so/elements/SoGLViewingMatrixElement.cpp \
    lib/database/src/so/elements/SoGLViewportRegionElement.cpp \
    lib/database/src/so/elements/SoInt32Element.cpp \
    lib/database/src/so/elements/SoLazyElement.cpp \
    lib/database/src/so/elements/SoLightAttenuationElement.cpp \
    lib/database/src/so/elements/SoLinePatternElement.cpp \
    lib/database/src/so/elements/SoLineWidthElement.cpp \
    lib/database/src/so/elements/SoLocalBBoxMatrixElement.cpp \
    lib/database/src/so/elements/SoMaterialBindingElement.cpp \
    lib/database/src/so/elements/SoModelMatrixElement.cpp \
    lib/database/src/so/elements/SoNormalBindingElement.cpp \
    lib/database/src/so/elements/SoNormalElement.cpp \
    lib/database/src/so/elements/SoOverrideElement.cpp \
    lib/database/src/so/elements/SoPickRayElement.cpp \
    lib/database/src/so/elements/SoPickStyleElement.cpp \
    lib/database/src/so/elements/SoPointSizeElement.cpp \
    lib/database/src/so/elements/SoProfileCoordinateElement.cpp \
    lib/database/src/so/elements/SoProfileElement.cpp \
    lib/database/src/so/elements/SoProjectionMatrixElement.cpp \
    lib/database/src/so/elements/SoReplacedElement.cpp \
    lib/database/src/so/elements/SoShapeHintsElement.cpp \
    lib/database/src/so/elements/SoShapeStyleElement.cpp \
    lib/database/src/so/elements/SoShininessElement.cpp \
    lib/database/src/so/elements/SoSpecularColorElement.cpp \
    lib/database/src/so/elements/SoSwitchElement.cpp \
    lib/database/src/so/elements/SoTextureCoordinateBindingElement.cpp \
    lib/database/src/so/elements/SoTextureCoordinateElement.cpp \
    lib/database/src/so/elements/SoTextureImageElement.cpp \
    lib/database/src/so/elements/SoTextureMatrixElement.cpp \
    lib/database/src/so/elements/SoTextureQualityElement.cpp \
    lib/database/src/so/elements/SoUnitsElement.cpp \
    lib/database/src/so/elements/SoViewVolumeElement.cpp \
    lib/database/src/so/elements/SoViewingMatrixElement.cpp \
    lib/database/src/so/elements/SoViewportRegionElement.cpp \
    lib/database/src/so/elements/SoWindowElement.cpp \
    lib/database/src/so/engines/SoBoolOperation.cpp \
    lib/database/src/so/engines/SoCalcExpr.cpp \
    lib/database/src/so/engines/SoCalculator.cpp \
    lib/database/src/so/engines/SoCompose.cpp \
    lib/database/src/so/engines/SoComputeBoundingBox.cpp \
    lib/database/src/so/engines/SoConcatenate.cpp \
    lib/database/src/so/engines/SoCounter.cpp \
    lib/database/src/so/engines/SoElapsedTime.cpp \
    lib/database/src/so/engines/SoEngine.cpp \
    lib/database/src/so/engines/SoEngineInit.cpp \
    lib/database/src/so/engines/SoFieldConverter.cpp \
    lib/database/src/so/engines/SoFieldConverters.cpp \
    lib/database/src/so/engines/SoGate.cpp \
    lib/database/src/so/engines/SoInterpolate.cpp \
    lib/database/src/so/engines/SoOnOff.cpp \
    lib/database/src/so/engines/SoOneShot.cpp \
    lib/database/src/so/engines/SoOutputData.cpp \
    lib/database/src/so/engines/SoSelectOne.cpp \
    lib/database/src/so/engines/SoTimeCounter.cpp \
    lib/database/src/so/engines/SoTransformVec3f.cpp \
    lib/database/src/so/engines/SoTriggerAny.cpp \
    lib/database/src/so/engines/SoUnknownEngine.cpp \
    lib/database/src/so/errors/SoDebugError.cpp \
    lib/database/src/so/errors/SoError.cpp \
    lib/database/src/so/errors/SoErrorInit.cpp \
    lib/database/src/so/errors/SoMemoryError.cpp \
    lib/database/src/so/errors/SoReadError.cpp \
    lib/database/src/so/events/SoButtonEvent.cpp \
    lib/database/src/so/events/SoEvent.cpp \
    lib/database/src/so/events/SoEventInit.cpp \
    lib/database/src/so/events/SoKeyboardEvent.cpp \
    lib/database/src/so/events/SoLocation2Event.cpp \
    lib/database/src/so/events/SoMotion3Event.cpp \
    lib/database/src/so/events/SoMouseButtonEvent.cpp \
    lib/database/src/so/events/SoSpaceballButtonEvent.cpp \
    lib/database/src/so/fields/SoField.cpp \
    lib/database/src/so/fields/SoFieldContainer.cpp \
    lib/database/src/so/fields/SoFieldData.cpp \
    lib/database/src/so/fields/SoFieldInit.cpp \
    lib/database/src/so/fields/SoGlobalField.cpp \
    lib/database/src/so/fields/SoMFBitMask.cpp \
    lib/database/src/so/fields/SoMFBool.cpp \
    lib/database/src/so/fields/SoMFColor.cpp \
    lib/database/src/so/fields/SoMFDouble.cpp \
    lib/database/src/so/fields/SoMFEngine.cpp \
    lib/database/src/so/fields/SoMFEnum.cpp \
    lib/database/src/so/fields/SoMFFloat.cpp \
    lib/database/src/so/fields/SoMFInt32.cpp \
    lib/database/src/so/fields/SoMFMatrix.cpp \
    lib/database/src/so/fields/SoMFMatrixd.cpp \
    lib/database/src/so/fields/SoMFName.cpp \
    lib/database/src/so/fields/SoMFNode.cpp \
    lib/database/src/so/fields/SoMFPath.cpp \
    lib/database/src/so/fields/SoMFPlane.cpp \
    lib/database/src/so/fields/SoMFPlaned.cpp \
    lib/database/src/so/fields/SoMFRotation.cpp \
    lib/database/src/so/fields/SoMFRotationd.cpp \
    lib/database/src/so/fields/SoMFShort.cpp \
    lib/database/src/so/fields/SoMFString.cpp \
    lib/database/src/so/fields/SoMFTime.cpp \
    lib/database/src/so/fields/SoMFUInt32.cpp \
    lib/database/src/so/fields/SoMFUShort.cpp \
    lib/database/src/so/fields/SoMFVec2f.cpp \
    lib/database/src/so/fields/SoMFVec2d.cpp \
    lib/database/src/so/fields/SoMFVec3f.cpp \
    lib/database/src/so/fields/SoMFVec3d.cpp \
    lib/database/src/so/fields/SoMFVec4f.cpp \
    lib/database/src/so/fields/SoMFVec4d.cpp \
    lib/database/src/so/fields/SoSFBitMask.cpp \
    lib/database/src/so/fields/SoSFBool.cpp \
    lib/database/src/so/fields/SoSFColor.cpp \
    lib/database/src/so/fields/SoSFDouble.cpp \
    lib/database/src/so/fields/SoSFEngine.cpp \
    lib/database/src/so/fields/SoSFEnum.cpp \
    lib/database/src/so/fields/SoSFFloat.cpp \
    lib/database/src/so/fields/SoSFImage.cpp \
    lib/database/src/so/fields/SoSFInt32.cpp \
    lib/database/src/so/fields/SoSFMatrix.cpp \
    lib/database/src/so/fields/SoSFMatrixd.cpp \
    lib/database/src/so/fields/SoSFName.cpp \
    lib/database/src/so/fields/SoSFNode.cpp \
    lib/database/src/so/fields/SoSFPath.cpp \
    lib/database/src/so/fields/SoSFPlane.cpp \
    lib/database/src/so/fields/SoSFPlaned.cpp \
    lib/database/src/so/fields/SoSFRotation.cpp \
    lib/database/src/so/fields/SoSFRotationd.cpp \
    lib/database/src/so/fields/SoSFShort.cpp \
    lib/database/src/so/fields/SoSFString.cpp \
    lib/database/src/so/fields/SoSFTime.cpp \
    lib/database/src/so/fields/SoSFTrigger.cpp \
    lib/database/src/so/fields/SoSFUInt32.cpp \
    lib/database/src/so/fields/SoSFUShort.cpp \
    lib/database/src/so/fields/SoSFVec2f.cpp \
    lib/database/src/so/fields/SoSFVec2d.cpp \
    lib/database/src/so/fields/SoSFVec3f.cpp \
    lib/database/src/so/fields/SoSFVec3d.cpp \
    lib/database/src/so/fields/SoSFVec4f.cpp \
    lib/database/src/so/fields/SoSFVec4d.cpp \
    lib/database/src/so/nodes/SoAnnotation.cpp \
    lib/database/src/so/nodes/SoArray.cpp \
    lib/database/src/so/nodes/SoAsciiText.cpp \
    lib/database/src/so/nodes/SoBaseColor.cpp \
    lib/database/src/so/nodes/SoBlinker.cpp \
    lib/database/src/so/nodes/SoCallback.cpp \
    lib/database/src/so/nodes/SoCamera.cpp \
    lib/database/src/so/nodes/SoClipPlane.cpp \
    lib/database/src/so/nodes/SoColorIndex.cpp \
    lib/database/src/so/nodes/SoComplexity.cpp \
    lib/database/src/so/nodes/SoCoordinate3.cpp \
    lib/database/src/so/nodes/SoCoordinate4.cpp \
    lib/database/src/so/nodes/SoDirectionalLight.cpp \
    lib/database/src/so/nodes/SoDrawStyle.cpp \
    lib/database/src/so/nodes/SoEnvironment.cpp \
    lib/database/src/so/nodes/SoEventCallback.cpp \
    lib/database/src/so/nodes/SoFile.cpp \
    lib/database/src/so/nodes/SoFont.cpp \
    lib/database/src/so/nodes/SoFontStyle.cpp \
    lib/database/src/so/nodes/SoGroup.cpp \
    lib/database/src/so/nodes/SoIndexedNurbsCurve.cpp \
    lib/database/src/so/nodes/SoIndexedNurbsSurface.cpp \
    lib/database/src/so/nodes/SoIndexedShape.cpp \
    lib/database/src/so/nodes/SoInfo.cpp \
    lib/database/src/so/nodes/SoLOD.cpp \
    lib/database/src/so/nodes/SoLabel.cpp \
    lib/database/src/so/nodes/SoLevelOfDetail.cpp \
    lib/database/src/so/nodes/SoLight.cpp \
    lib/database/src/so/nodes/SoLightModel.cpp \
    lib/database/src/so/nodes/SoLinearProfile.cpp \
    lib/database/src/so/nodes/SoLocateHighlight.cpp \
    lib/database/src/so/nodes/SoMaterial.cpp \
    lib/database/src/so/nodes/SoMaterialBinding.cpp \
    lib/database/src/so/nodes/SoMatrixTransform.cpp \
    lib/database/src/so/nodes/SoMultipleCopy.cpp \
    lib/database/src/so/nodes/SoNode.cpp \
    lib/database/src/so/nodes/SoNodeInit.cpp \
    lib/database/src/so/nodes/SoNonIndexedShape.cpp \
    lib/database/src/so/nodes/SoNormal.cpp \
    lib/database/src/so/nodes/SoNormalBinding.cpp \
    lib/database/src/so/nodes/SoNurbsCurve.cpp \
    lib/database/src/so/nodes/SoNurbsProfile.cpp \
    lib/database/src/so/nodes/SoNurbsSurface.cpp \
    lib/database/src/so/nodes/SoOrthographicCamera.cpp \
    lib/database/src/so/nodes/SoPackedColor.cpp \
    lib/database/src/so/nodes/SoPathSwitch.cpp \
    lib/database/src/so/nodes/SoPendulum.cpp \
    lib/database/src/so/nodes/SoPerspectiveCamera.cpp \
    lib/database/src/so/nodes/SoPickStyle.cpp \
    lib/database/src/so/nodes/SoPointLight.cpp \
    lib/database/src/so/nodes/SoPointSet.cpp \
    lib/database/src/so/nodes/SoProfile.cpp \
    lib/database/src/so/nodes/SoProfileCoordinate2.cpp \
    lib/database/src/so/nodes/SoProfileCoordinate3.cpp \
    lib/database/src/so/nodes/SoResetTransform.cpp \
    lib/database/src/so/nodes/SoRotation.cpp \
    lib/database/src/so/nodes/SoRotationXYZ.cpp \
    lib/database/src/so/nodes/SoRotor.cpp \
    lib/database/src/so/nodes/SoScale.cpp \
    lib/database/src/so/nodes/SoSeparator.cpp \
    lib/database/src/so/nodes/SoShape.cpp \
    lib/database/src/so/nodes/SoShapeHints.cpp \
    lib/database/src/so/nodes/SoShuttle.cpp \
    lib/database/src/so/nodes/SoSpotLight.cpp \
    lib/database/src/so/nodes/SoSwitch.cpp \
    lib/database/src/so/nodes/SoText2.cpp \
    lib/database/src/so/nodes/SoText3.cpp \
    lib/database/src/so/nodes/SoTexture2.cpp \
    lib/database/src/so/nodes/SoTexture2Transform.cpp \
    lib/database/src/so/nodes/SoTextureCoordinate2.cpp \
    lib/database/src/so/nodes/SoTextureCoordinateBinding.cpp \
    lib/database/src/so/nodes/SoTextureCoordinateDefault.cpp \
    lib/database/src/so/nodes/SoTextureCoordinateEnvironment.cpp \
    lib/database/src/so/nodes/SoTextureCoordinateFunction.cpp \
    lib/database/src/so/nodes/SoTextureCoordinatePlane.cpp \
    lib/database/src/so/nodes/SoTransform.cpp \
    lib/database/src/so/nodes/SoTransformSeparator.cpp \
    lib/database/src/so/nodes/SoTransformation.cpp \
    lib/database/src/so/nodes/SoTranslation.cpp \
    lib/database/src/so/nodes/SoUnits.cpp \
    lib/database/src/so/nodes/SoUnknownNode.cpp \
    lib/database/src/so/nodes/SoVertexProperty.cpp \
    lib/database/src/so/nodes/SoVertexShape.cpp \
    lib/database/src/so/nodes/SoWWWAnchor.cpp \
    lib/database/src/so/nodes/SoWWWInline.cpp \
    lib/database/src/so/nodes/nurbs/SoCurveMaps.cpp \
    lib/database/src/so/nodes/nurbs/SoCurveRender.cpp \
    lib/database/src/so/nodes/nurbs/SoCurveRndEvl.cpp \
    lib/database/src/so/nodes/nurbs/SoGLRender.cpp \
    lib/database/src/so/nodes/nurbs/SoPickEvals.cpp \
    lib/database/src/so/nodes/nurbs/SoPickMaps.cpp \
    lib/database/src/so/nodes/nurbs/SoPickRender.cpp \
    lib/database/src/so/nodes/nurbs/SoPrimEvals.cpp \
    lib/database/src/so/nodes/nurbs/SoPrimMaps.cpp \
    lib/database/src/so/nodes/nurbs/SoPrimRender.cpp \
    lib/database/src/so/nodes/nurbs/clients/nurbsClient.cpp \
    lib/database/src/so/nodes/nurbs/libnurbs/libNurbs.cpp \
    lib/database/src/so/sensors/SoAlarmSensor.cpp \
    lib/database/src/so/sensors/SoDataSensor.cpp \
    lib/database/src/so/sensors/SoDelayQueueSensor.cpp \
    lib/database/src/so/sensors/SoFieldSensor.cpp \
    lib/database/src/so/sensors/SoIdleSensor.cpp \
    lib/database/src/so/sensors/SoNodeSensor.cpp \
    lib/database/src/so/sensors/SoOneShotSensor.cpp \
    lib/database/src/so/sensors/SoPathSensor.cpp \
    lib/database/src/so/sensors/SoSensor.cpp \
    lib/database/src/so/sensors/SoSensorManager.cpp \
    lib/database/src/so/sensors/SoTimerQueueSensor.cpp \
    lib/database/src/so/sensors/SoTimerSensor.cpp \
    lib/database/src/so/upgraders/SoUpgrader.cpp \
    lib/database/src/so/upgraders/SoUpgraderInit.cpp \
    lib/database/src/so/upgraders/SoV1CustomNode.cpp \
    lib/database/src/so/upgraders/SoV1DrawStyle.cpp \
    lib/database/src/so/upgraders/SoV1Environment.cpp \
    lib/database/src/so/upgraders/SoV1IndexedTriangleMesh.cpp \
    lib/database/src/so/upgraders/SoV1LayerGroup.cpp \
    lib/database/src/so/upgraders/SoV1LightModel.cpp \
    lib/database/src/so/upgraders/SoV1Material.cpp \
    lib/database/src/so/upgraders/SoV1PackedColor.cpp \
    lib/database/src/so/upgraders/SoV1PickStyle.cpp \
    lib/database/src/so/upgraders/SoV1Separator.cpp \
    lib/database/src/so/upgraders/SoV1ShapeHints.cpp \
    lib/database/src/so/upgraders/SoV1Text2.cpp \
    lib/database/src/so/upgraders/SoV1Text3.cpp \
    lib/database/src/so/upgraders/SoV1Texture2.cpp \
    lib/database/src/so/upgraders/SoV1TextureCoordinateCube.cpp \
    lib/database/src/so/upgraders/SoV1TextureCoordinateCylinder.cpp \
    lib/database/src/so/upgraders/SoV1TextureCoordinateEnvironment.cpp \
    lib/database/src/so/upgraders/SoV1TextureCoordinatePlane.cpp \
    lib/database/src/so/upgraders/SoV1TextureCoordinateSphere.cpp \
    lib/database/src/so/upgraders/SoV2AsciiText.cpp \
    lib/database/src/so/upgraders/SoV2FontStyle.cpp \
    lib/database/src/so/upgraders/SoV2LOD.cpp \
    lib/database/src/so/upgraders/SoV2MaterialIndex.cpp \
    lib/database/src/so/upgraders/SoV2Text2.cpp \
    lib/database/src/so/upgraders/SoV2Text3.cpp \
    lib/database/src/so/upgraders/SoV2VertexProperty.cpp \
    lib/database/src/so/upgraders/SoV2WWWAnchor.cpp \
    lib/database/src/so/upgraders/SoV2WWWInline.cpp \
    lib/database/src/so/linux.cpp \
    lib/interaction/src/SoInteraction.cpp \
    lib/interaction/src/SoSceneManager.cpp \
    lib/interaction/src/SoSelection.cpp \
    lib/interaction/src/actions/SoBoxHighlightRenderAction.cpp \
    lib/interaction/src/actions/SoLineHighlightRenderAction.cpp \
    lib/interaction/src/draggers/SoCenterballDragger.cpp \
    lib/interaction/src/draggers/SoDirectionalLightDragger.cpp \
    lib/interaction/src/draggers/SoDragPointDragger.cpp \
    lib/interaction/src/draggers/SoDragger.cpp \
    lib/interaction/src/draggers/SoDraggerInit.cpp \
    lib/interaction/src/draggers/SoHandleBoxDragger.cpp \
    lib/interaction/src/draggers/SoJackDragger.cpp \
    lib/interaction/src/draggers/SoPointLightDragger.cpp \
    lib/interaction/src/draggers/SoRotateCylindricalDragger.cpp \
    lib/interaction/src/draggers/SoRotateDiscDragger.cpp \
    lib/interaction/src/draggers/SoRotateSphericalDragger.cpp \
    lib/interaction/src/draggers/SoScale1Dragger.cpp \
    lib/interaction/src/draggers/SoScale2Dragger.cpp \
    lib/interaction/src/draggers/SoScale2UniformDragger.cpp \
    lib/interaction/src/draggers/SoScaleUniformDragger.cpp \
    lib/interaction/src/draggers/SoSpotLightDragger.cpp \
    lib/interaction/src/draggers/SoTabBoxDragger.cpp \
    lib/interaction/src/draggers/SoTabPlaneDragger.cpp \
    lib/interaction/src/draggers/SoTrackballDragger.cpp \
    lib/interaction/src/draggers/SoTransformBoxDragger.cpp \
    lib/interaction/src/draggers/SoTransformerDragger.cpp \
    lib/interaction/src/draggers/SoTranslate1Dragger.cpp \
    lib/interaction/src/draggers/SoTranslate2Dragger.cpp \
    lib/interaction/src/manips/SoCenterballManip.cpp \
    lib/interaction/src/manips/SoDirectionalLightManip.cpp \
    lib/interaction/src/manips/SoHandleBoxManip.cpp \
    lib/interaction/src/manips/SoJackManip.cpp \
    lib/interaction/src/manips/SoPointLightManip.cpp \
    lib/interaction/src/manips/SoSpotLightManip.cpp \
    lib/interaction/src/manips/SoTabBoxManip.cpp \
    lib/interaction/src/manips/SoTrackBallManip.cpp \
    lib/interaction/src/manips/SoTransformManip.cpp \
    lib/interaction/src/manips/SoTransformerManip.cpp \
    lib/interaction/src/manips/SoTransformBoxManip.cpp \
    lib/interaction/src/nodekits/SoInteractionKit.cpp \
    lib/interaction/src/nodes/SoAntiSquish.cpp \
    lib/interaction/src/nodes/SoSurroundScale.cpp \
    lib/nodekits/src/SoNodeKitPath.cpp \
    lib/nodekits/src/details/SoNodeKitDetail.cpp \
    lib/nodekits/src/nodekits/SoAppearKit.cpp \
    lib/nodekits/src/nodekits/SoBaseKit.cpp \
    lib/nodekits/src/nodekits/SoCameraKit.cpp \
    lib/nodekits/src/nodekits/SoLightKit.cpp \
    lib/nodekits/src/nodekits/SoNkCatalog.cpp \
    lib/nodekits/src/nodekits/SoNkParts.cpp \
    lib/nodekits/src/nodekits/SoNodeKit.cpp \
    lib/nodekits/src/nodekits/SoNodeKitListPart.cpp \
    lib/nodekits/src/nodekits/SoSceneKit.cpp \
    lib/nodekits/src/nodekits/SoSeparatorKit.cpp \
    lib/nodekits/src/nodekits/SoShapeKit.cpp \
    lib/nodekits/src/nodekits/SoWrapperKit.cpp \
    lib/nodekits/src/upgraders/SoV1AppearKit.cpp \
    lib/nodekits/src/upgraders/SoV1BaseKit.cpp \
    lib/nodekits/src/upgraders/SoV1CameraKit.cpp \
    lib/nodekits/src/upgraders/SoV1ConeKit.cpp \
    lib/nodekits/src/upgraders/SoV1CubeKit.cpp \
    lib/nodekits/src/upgraders/SoV1CylinderKit.cpp \
    lib/nodekits/src/upgraders/SoV1DirLtKit.cpp \
    lib/nodekits/src/upgraders/SoV1FaceSetKit.cpp \
    lib/nodekits/src/upgraders/SoV1GroupKit.cpp \
    lib/nodekits/src/upgraders/SoV1IFaceSetKit.cpp \
    lib/nodekits/src/upgraders/SoV1ILineSetKit.cpp \
    lib/nodekits/src/upgraders/SoV1INrbCurvKit.cpp \
    lib/nodekits/src/upgraders/SoV1INrbSurfKit.cpp \
    lib/nodekits/src/upgraders/SoV1ITMshSetKit.cpp \
    lib/nodekits/src/upgraders/SoV1LightKit.cpp \
    lib/nodekits/src/upgraders/SoV1LineSetKit.cpp \
    lib/nodekits/src/upgraders/SoV1NkCatalog.cpp \
    lib/nodekits/src/upgraders/SoV1NkParts.cpp \
    lib/nodekits/src/upgraders/SoV1NodeKit.cpp \
    lib/nodekits/src/upgraders/SoV1NurbCurvKit.cpp \
    lib/nodekits/src/upgraders/SoV1NurbSurfKit.cpp \
    lib/nodekits/src/upgraders/SoV1OrthoCamKit.cpp \
    lib/nodekits/src/upgraders/SoV1PerspCamKit.cpp \
    lib/nodekits/src/upgraders/SoV1PointLtKit.cpp \
    lib/nodekits/src/upgraders/SoV1PointSetKit.cpp \
    lib/nodekits/src/upgraders/SoV1QuadMeshKit.cpp \
    lib/nodekits/src/upgraders/SoV1SceneKit.cpp \
    lib/nodekits/src/upgraders/SoV1ShapeKit.cpp \
    lib/nodekits/src/upgraders/SoV1SphereKit.cpp \
    lib/nodekits/src/upgraders/SoV1SpotLtKit.cpp \
    lib/nodekits/src/upgraders/SoV1Text2Kit.cpp \
    lib/nodekits/src/upgraders/SoV1Text3Kit.cpp \
    lib/nodekits/src/upgraders/SoV1TriStripKit.cpp \
    lib/nodekits/src/upgraders/SoV1VtxShapeKit.cpp \
    lib/nodekits/src/upgraders/SoV1WrapperKit.cpp

# PPP Generated sources
SOURCES += \
    lib/database/src/so/nodes/SoCone.cpp \
    lib/database/src/so/nodes/SoCube.cpp \
    lib/database/src/so/nodes/SoCylinder.cpp \
    lib/database/src/so/nodes/SoFaceSet.cpp \
    lib/database/src/so/nodes/SoIndexedFaceSet.cpp \
    lib/database/src/so/nodes/SoIndexedTriangleSet.cpp \
    lib/database/src/so/nodes/SoIndexedLineSet.cpp \
    lib/database/src/so/nodes/SoIndexedTriangleStripSet.cpp \
    lib/database/src/so/nodes/SoLineSet.cpp \
    lib/database/src/so/nodes/SoQuadMesh.cpp \
    lib/database/src/so/nodes/SoSphere.cpp \
    lib/database/src/so/nodes/SoTriangleStripSet.cpp

# image loaders
SOURCES += \
    lib/database/src/so/nodes/readGIF.c \
    lib/database/src/so/nodes/readJPEG.cpp

RELATEDFILES += \
    doxygen/MainPage.h \
    doxygen/Groups.h \
    doxygen/Performance.h \
    doxygen/Environment.h \
    doxygen/ReleaseNotes.h \
    lib/database/src/so/nodes/SoCone.pp \
    lib/database/src/so/nodes/SoCube.pp \
    lib/database/src/so/nodes/SoCylinder.pp \
    lib/database/src/so/nodes/SoFaceSet.pp \
    lib/database/src/so/nodes/SoIndexedFaceSet.pp \
    lib/database/src/so/nodes/SoIndexedTriangleSet.pp \
    lib/database/src/so/nodes/SoIndexedLineSet.pp \
    lib/database/src/so/nodes/SoIndexedTriangleStripSet.pp \
    lib/database/src/so/nodes/SoLineSet.pp \
    lib/database/src/so/nodes/SoPointSet.pp \
    lib/database/src/so/nodes/SoQuadMesh.pp \
    lib/database/src/so/nodes/SoSphere.pp \
    lib/database/src/so/nodes/SoTriangleStripSet.pp    

win32::SOURCES += \
    lib/initDll.cpp

win32::RC_FILE += \
    lib/resource.rc

# mac framework config
macx::lib_bundle {
  QMAKE_FRAMEWORK_BUNDLE_NAME = Inventor
  QMAKE_FRAMEWORK_VERSION = 2.5

  # add Inventor -> . link to PrivateHeaders/
  FRAMEWORK_PRIVHEADERS1.version = Versions
  FRAMEWORK_PRIVHEADERS1.path = PrivateHeaders
  FRAMEWORK_PRIVHEADERS1.files = \
    build/macx/Inventor
  QMAKE_BUNDLE_DATA += FRAMEWORK_PRIVHEADERS1

  # add Inventor -> . link to Headers/
  FRAMEWORK_HEADERS1.version = Versions
  FRAMEWORK_HEADERS1.path = Headers
  FRAMEWORK_HEADERS1.files = \
    build/macx/Inventor
  QMAKE_BUNDLE_DATA += FRAMEWORK_HEADERS1

  # private header files
  FRAMEWORK_PRIVHEADERS2.version = Versions
  FRAMEWORK_PRIVHEADERS2.path = PrivateHeaders
  FRAMEWORK_PRIVHEADERS2.files = \
    lib/database/include/SoUnknownEngine.h \
    lib/database/include/SoUnknownNode.h
  QMAKE_BUNDLE_DATA += FRAMEWORK_PRIVHEADERS2

  FRAMEWORK_HEADERS2.version = Versions
  FRAMEWORK_HEADERS2.path = Headers
  FRAMEWORK_HEADERS2.files = \
    lib/mevis/include/Inventor/SoCatch.h \
    lib/mevis/include/Inventor/SoInventor.h \
    lib/mevis/include/Inventor/SoMeVis.h \
    lib/mevis/include/Inventor/SoTrace.h \
    lib/mevis/include/Inventor/SoProfiling.h \
    lib/database/include/Inventor/SoMachine.h \
    lib/database/include/Inventor/SoDebug.h \
    lib/database/include/Inventor/Sb.h \
    lib/database/include/Inventor/SbBasic.h \
    lib/database/include/Inventor/SbBox.h \
    lib/database/include/Inventor/SbBoxd.h \
    lib/database/include/Inventor/SbColor.h \
    lib/database/include/Inventor/SbDict.h \
    lib/database/include/Inventor/SbLinear.h \
    lib/database/include/Inventor/SbLineard.h \
    lib/database/include/Inventor/SbPList.h \
    lib/database/include/Inventor/SbString.h \
    lib/database/include/Inventor/SbTime.h \
    lib/database/include/Inventor/SbTypeDefs.h \
    lib/database/include/Inventor/SbViewportRegion.h \
    lib/database/include/Inventor/So.h \
    lib/database/include/Inventor/SoDB.h \
    lib/database/include/Inventor/SoInput.h \
    lib/database/include/Inventor/SoLists.h \
    lib/database/include/Inventor/SoOutput.h \
    lib/database/include/Inventor/SoPath.h \
    lib/database/include/Inventor/SoPickedPoint.h \
    lib/database/include/Inventor/SoPrimitiveVertex.h \
    lib/database/include/Inventor/SoType.h \
    lib/interaction/include/Inventor/SoInteraction.h \
    lib/interaction/include/Inventor/SoSceneManager.h \
    lib/nodekits/include/Inventor/SoNodeKitPath.h
  QMAKE_BUNDLE_DATA += FRAMEWORK_HEADERS2

  FRAMEWORK_HEADERS3.path = Headers/actions
  FRAMEWORK_HEADERS3.files = \
    lib/database/include/Inventor/actions/SoAction.h \
    lib/database/include/Inventor/actions/SoActions.h \
    lib/database/include/Inventor/actions/SoCallbackAction.h \
    lib/database/include/Inventor/actions/SoGetBoundingBoxAction.h \
    lib/database/include/Inventor/actions/SoGetMatrixAction.h \
    lib/database/include/Inventor/actions/SoGLRenderAction.h \
    lib/database/include/Inventor/actions/SoHandleEventAction.h \
    lib/database/include/Inventor/actions/SoPickAction.h \
    lib/database/include/Inventor/actions/SoRayPickAction.h \
    lib/database/include/Inventor/actions/SoSearchAction.h \
    lib/database/include/Inventor/actions/SoSubAction.h \
    lib/database/include/Inventor/actions/SoWriteAction.h \
    lib/interaction/include/Inventor/actions/SoBoxHighlightRenderAction.h \
    lib/interaction/include/Inventor/actions/SoLineHighlightRenderAction.h
  QMAKE_BUNDLE_DATA += FRAMEWORK_HEADERS3

  FRAMEWORK_HEADERS4.path = Headers/bundles
  FRAMEWORK_HEADERS4.files = \
    lib/database/include/Inventor/bundles/SoBundle.h \
    lib/database/include/Inventor/bundles/SoMaterialBundle.h \
    lib/database/include/Inventor/bundles/SoNormalBundle.h \
    lib/database/include/Inventor/bundles/SoTextureCoordinateBundle.h
  QMAKE_BUNDLE_DATA += FRAMEWORK_HEADERS4

  FRAMEWORK_HEADERS5.path = Headers/caches
  FRAMEWORK_HEADERS5.files = \
    lib/database/include/Inventor/caches/SoBoundingBoxCache.h \
    lib/database/include/Inventor/caches/SoCache.h \
    lib/database/include/Inventor/caches/SoGLCacheList.h \
    lib/database/include/Inventor/caches/SoGLRenderCache.h \
    lib/database/include/Inventor/caches/SoNormalCache.h
  QMAKE_BUNDLE_DATA += FRAMEWORK_HEADERS5

  FRAMEWORK_HEADERS6.path = Headers/details
  FRAMEWORK_HEADERS6.files = \
    lib/database/include/Inventor/details/SoConeDetail.h \
    lib/database/include/Inventor/details/SoCubeDetail.h \
    lib/database/include/Inventor/details/SoCylinderDetail.h \
    lib/database/include/Inventor/details/SoDetail.h \
    lib/database/include/Inventor/details/SoDetails.h \
    lib/database/include/Inventor/details/SoFaceDetail.h \
    lib/database/include/Inventor/details/SoLineDetail.h \
    lib/database/include/Inventor/details/SoPointDetail.h \
    lib/database/include/Inventor/details/SoSubDetail.h \
    lib/database/include/Inventor/details/SoTextDetail.h \
    lib/nodekits/include/Inventor/details/SoNodeKitDetail.h
  QMAKE_BUNDLE_DATA += FRAMEWORK_HEADERS6

  FRAMEWORK_HEADERS7.path = Headers/draggers
  FRAMEWORK_HEADERS7.files = \
    lib/interaction/include/Inventor/draggers/SoRotateSphericalDragger.h \
    lib/interaction/include/Inventor/draggers/SoCenterballDragger.h \
    lib/interaction/include/Inventor/draggers/SoDirectionalLightDragger.h \
    lib/interaction/include/Inventor/draggers/SoDragger.h \
    lib/interaction/include/Inventor/draggers/SoDragPointDragger.h \
    lib/interaction/include/Inventor/draggers/SoHandleBoxDragger.h \
    lib/interaction/include/Inventor/draggers/SoJackDragger.h \
    lib/interaction/include/Inventor/draggers/SoPointLightDragger.h \
    lib/interaction/include/Inventor/draggers/SoRotateCylindricalDragger.h \
    lib/interaction/include/Inventor/draggers/SoRotateDiscDragger.h \
    lib/interaction/include/Inventor/draggers/SoScale1Dragger.h \
    lib/interaction/include/Inventor/draggers/SoScale2Dragger.h \
    lib/interaction/include/Inventor/draggers/SoScale2UniformDragger.h \
    lib/interaction/include/Inventor/draggers/SoScaleUniformDragger.h \
    lib/interaction/include/Inventor/draggers/SoSpotLightDragger.h \
    lib/interaction/include/Inventor/draggers/SoTabBoxDragger.h \
    lib/interaction/include/Inventor/draggers/SoTabPlaneDragger.h \
    lib/interaction/include/Inventor/draggers/SoTrackballDragger.h \
    lib/interaction/include/Inventor/draggers/SoTransformBoxDragger.h \
    lib/interaction/include/Inventor/draggers/SoTransformerDragger.h \
    lib/interaction/include/Inventor/draggers/SoTranslate1Dragger.h \
    lib/interaction/include/Inventor/draggers/SoTranslate2Dragger.h
  QMAKE_BUNDLE_DATA += FRAMEWORK_HEADERS7

  FRAMEWORK_HEADERS8.path = Headers/elements
  FRAMEWORK_HEADERS8.files = \
    lib/mevis/include/Inventor/elements/SoGLPolygonOffsetElement.h \
    lib/mevis/include/Inventor/elements/SoGLStencilBitsElement.h \
    lib/mevis/include/Inventor/elements/SoPolygonOffsetElement.h \
    lib/mevis/include/Inventor/elements/SoStencilBitsElement.h \
    lib/database/include/Inventor/elements/SoAccumulatedElement.h \
    lib/database/include/Inventor/elements/SoAmbientColorElement.h \
    lib/database/include/Inventor/elements/SoBBoxModelMatrixElement.h \
    lib/database/include/Inventor/elements/SoCacheElement.h \
    lib/database/include/Inventor/elements/SoClipPlaneElement.h \
    lib/database/include/Inventor/elements/SoComplexityElement.h \
    lib/database/include/Inventor/elements/SoComplexityTypeElement.h \
    lib/database/include/Inventor/elements/SoCoordinateElement.h \
    lib/database/include/Inventor/elements/SoCreaseAngleElement.h \
    lib/database/include/Inventor/elements/SoCullVolumeElement.h \
    lib/database/include/Inventor/elements/SoCurrentGLMaterialElement.h \
    lib/database/include/Inventor/elements/SoDiffuseColorElement.h \
    lib/database/include/Inventor/elements/SoDrawStyleElement.h \
    lib/database/include/Inventor/elements/SoElement.h \
    lib/database/include/Inventor/elements/SoElements.h \
    lib/database/include/Inventor/elements/SoEmissiveColorElement.h \
    lib/database/include/Inventor/elements/SoFloatElement.h \
    lib/database/include/Inventor/elements/SoFocalDistanceElement.h \
    lib/database/include/Inventor/elements/SoFontNameElement.h \
    lib/database/include/Inventor/elements/SoFontSizeElement.h \
    lib/database/include/Inventor/elements/SoGLAmbientColorElement.h \
    lib/database/include/Inventor/elements/SoGLCacheContextElement.h \
    lib/database/include/Inventor/elements/SoGLClipPlaneElement.h \
    lib/database/include/Inventor/elements/SoGLColorIndexElement.h \
    lib/database/include/Inventor/elements/SoGLCoordinateElement.h \
    lib/database/include/Inventor/elements/SoGLDiffuseColorElement.h \
    lib/database/include/Inventor/elements/SoGLDrawStyleElement.h \
    lib/database/include/Inventor/elements/SoGLEmissiveColorElement.h \
    lib/database/include/Inventor/elements/SoGLLazyElement.h \
    lib/database/include/Inventor/elements/SoGLLightIdElement.h \
    lib/database/include/Inventor/elements/SoGLLightModelElement.h \
    lib/database/include/Inventor/elements/SoGLLinePatternElement.h \
    lib/database/include/Inventor/elements/SoGLLineWidthElement.h \
    lib/database/include/Inventor/elements/SoGLModelMatrixElement.h \
    lib/database/include/Inventor/elements/SoGLNormalElement.h \
    lib/database/include/Inventor/elements/SoGLPointSizeElement.h \
    lib/database/include/Inventor/elements/SoGLPolygonStippleElement.h \
    lib/database/include/Inventor/elements/SoGLProjectionMatrixElement.h \
    lib/database/include/Inventor/elements/SoGLRenderPassElement.h \
    lib/database/include/Inventor/elements/SoGLShapeHintsElement.h \
    lib/database/include/Inventor/elements/SoGLShininessElement.h \
    lib/database/include/Inventor/elements/SoGLSpecularColorElement.h \
    lib/database/include/Inventor/elements/SoGLTextureBlendColorElement.h \
    lib/database/include/Inventor/elements/SoGLTextureCoordinateElement.h \
    lib/database/include/Inventor/elements/SoGLTextureEnabledElement.h \
    lib/database/include/Inventor/elements/SoGLTextureImageElement.h \
    lib/database/include/Inventor/elements/SoGLTextureMatrixElement.h \
    lib/database/include/Inventor/elements/SoGLTextureModelElement.h \
    lib/database/include/Inventor/elements/SoGLTextureQualityElement.h \
    lib/database/include/Inventor/elements/SoGLTextureWrapSElement.h \
    lib/database/include/Inventor/elements/SoGLTextureWrapTElement.h \
    lib/database/include/Inventor/elements/SoGLUpdateAreaElement.h \
    lib/database/include/Inventor/elements/SoGLViewingMatrixElement.h \
    lib/database/include/Inventor/elements/SoGLViewportRegionElement.h \
    lib/database/include/Inventor/elements/SoInt32Element.h \
    lib/database/include/Inventor/elements/SoLazyElement.h \
    lib/database/include/Inventor/elements/SoLightAttenuationElement.h \
    lib/database/include/Inventor/elements/SoLightModelElement.h \
    lib/database/include/Inventor/elements/SoLinePatternElement.h \
    lib/database/include/Inventor/elements/SoLineWidthElement.h \
    lib/database/include/Inventor/elements/SoLocalBBoxMatrixElement.h \
    lib/database/include/Inventor/elements/SoLongElement.h \
    lib/database/include/Inventor/elements/SoMaterialBindingElement.h \
    lib/database/include/Inventor/elements/SoModelMatrixElement.h \
    lib/database/include/Inventor/elements/SoNormalBindingElement.h \
    lib/database/include/Inventor/elements/SoNormalElement.h \
    lib/database/include/Inventor/elements/SoOverrideElement.h \
    lib/database/include/Inventor/elements/SoPickRayElement.h \
    lib/database/include/Inventor/elements/SoPickStyleElement.h \
    lib/database/include/Inventor/elements/SoPointSizeElement.h \
    lib/database/include/Inventor/elements/SoProfileCoordinateElement.h \
    lib/database/include/Inventor/elements/SoProfileElement.h \
    lib/database/include/Inventor/elements/SoProjectionMatrixElement.h \
    lib/database/include/Inventor/elements/SoReplacedElement.h \
    lib/database/include/Inventor/elements/SoShapeHintsElement.h \
    lib/database/include/Inventor/elements/SoShapeStyleElement.h \
    lib/database/include/Inventor/elements/SoShininessElement.h \
    lib/database/include/Inventor/elements/SoSpecularColorElement.h \
    lib/database/include/Inventor/elements/SoSubElement.h \
    lib/database/include/Inventor/elements/SoSwitchElement.h \
    lib/database/include/Inventor/elements/SoTextureBlendColorElement.h \
    lib/database/include/Inventor/elements/SoTextureCoordinateBindingElement.h \
    lib/database/include/Inventor/elements/SoTextureCoordinateElement.h \
    lib/database/include/Inventor/elements/SoTextureImageElement.h \
    lib/database/include/Inventor/elements/SoTextureMatrixElement.h \
    lib/database/include/Inventor/elements/SoTextureModelElement.h \
    lib/database/include/Inventor/elements/SoTextureQualityElement.h \
    lib/database/include/Inventor/elements/SoTextureWrapSElement.h \
    lib/database/include/Inventor/elements/SoTextureWrapTElement.h \
    lib/database/include/Inventor/elements/SoTransparencyElement.h \
    lib/database/include/Inventor/elements/SoUnitsElement.h \
    lib/database/include/Inventor/elements/SoViewVolumeElement.h \
    lib/database/include/Inventor/elements/SoViewingMatrixElement.h \
    lib/database/include/Inventor/elements/SoViewportRegionElement.h \
    lib/database/include/Inventor/elements/SoWindowElement.h
  QMAKE_BUNDLE_DATA += FRAMEWORK_HEADERS8

  FRAMEWORK_HEADERS9.path = Headers/engines
  FRAMEWORK_HEADERS9.files = \
    lib/database/include/Inventor/engines/SoBoolOperation.h \
    lib/database/include/Inventor/engines/SoCalculator.h \
    lib/database/include/Inventor/engines/SoCompose.h \
    lib/database/include/Inventor/engines/SoComputeBoundingBox.h \
    lib/database/include/Inventor/engines/SoConcatenate.h \
    lib/database/include/Inventor/engines/SoCounter.h \
    lib/database/include/Inventor/engines/SoElapsedTime.h \
    lib/database/include/Inventor/engines/SoEngine.h \
    lib/database/include/Inventor/engines/SoEngines.h \
    lib/database/include/Inventor/engines/SoFieldConverter.h \
    lib/database/include/Inventor/engines/SoGate.h \
    lib/database/include/Inventor/engines/SoInterpolate.h \
    lib/database/include/Inventor/engines/SoOneShot.h \
    lib/database/include/Inventor/engines/SoOnOff.h \
    lib/database/include/Inventor/engines/SoOutputData.h \
    lib/database/include/Inventor/engines/SoSelectOne.h \
    lib/database/include/Inventor/engines/SoSubEngine.h \
    lib/database/include/Inventor/engines/SoTimeCounter.h \
    lib/database/include/Inventor/engines/SoTransformVec3f.h \
    lib/database/include/Inventor/engines/SoTriggerAny.h
  QMAKE_BUNDLE_DATA += FRAMEWORK_HEADERS9

  FRAMEWORK_HEADERS10.path = Headers/errors
  FRAMEWORK_HEADERS10.files = \
    lib/database/include/Inventor/errors/SoDebugError.h \
    lib/database/include/Inventor/errors/SoError.h \
    lib/database/include/Inventor/errors/SoErrors.h \
    lib/database/include/Inventor/errors/SoMemoryError.h \
    lib/database/include/Inventor/errors/SoReadError.h
  QMAKE_BUNDLE_DATA += FRAMEWORK_HEADERS10

  FRAMEWORK_HEADERS11.path = Headers/events
  FRAMEWORK_HEADERS11.files = \
    lib/mevis/include/Inventor/events/SoMouseWheelEvent.h \
    lib/mevis/include/Inventor/events/SoLocation2RefreshEvent.h \
    lib/database/include/Inventor/events/SoButtonEvent.h \
    lib/database/include/Inventor/events/SoEvent.h \
    lib/database/include/Inventor/events/SoEvents.h \
    lib/database/include/Inventor/events/SoKeyboardEvent.h \
    lib/database/include/Inventor/events/SoLocation2Event.h \
    lib/database/include/Inventor/events/SoMotion3Event.h \
    lib/database/include/Inventor/events/SoMouseButtonEvent.h \
    lib/database/include/Inventor/events/SoSpaceballButtonEvent.h \
    lib/database/include/Inventor/events/SoSubEvent.h
  QMAKE_BUNDLE_DATA += FRAMEWORK_HEADERS11

  FRAMEWORK_HEADERS12.path = Headers/fields
  FRAMEWORK_HEADERS12.files = \
    lib/mevis/include/Inventor/fields/SoSFVec2s.h \
    lib/mevis/include/Inventor/fields/SoSFVec3s.h \
    lib/database/include/Inventor/fields/SoMFLong.h \
    lib/database/include/Inventor/fields/SoMFUShort.h \
    lib/database/include/Inventor/fields/SoSFInt32.h \
    lib/database/include/Inventor/fields/SoField.h \
    lib/database/include/Inventor/fields/SoFieldContainer.h \
    lib/database/include/Inventor/fields/SoFieldData.h \
    lib/database/include/Inventor/fields/SoFields.h \
    lib/database/include/Inventor/fields/SoMFBitMask.h \
    lib/database/include/Inventor/fields/SoMFBool.h \
    lib/database/include/Inventor/fields/SoMFColor.h \
    lib/database/include/Inventor/fields/SoMFDouble.h \
    lib/database/include/Inventor/fields/SoMFEngine.h \
    lib/database/include/Inventor/fields/SoMFEnum.h \
    lib/database/include/Inventor/fields/SoMFFloat.h \
    lib/database/include/Inventor/fields/SoMFInt32.h \
    lib/database/include/Inventor/fields/SoMFMatrix.h \
    lib/database/include/Inventor/fields/SoMFMatrixd.h \
    lib/database/include/Inventor/fields/SoMFName.h \
    lib/database/include/Inventor/fields/SoMFNode.h \
    lib/database/include/Inventor/fields/SoMFPath.h \
    lib/database/include/Inventor/fields/SoMFPlane.h \
    lib/database/include/Inventor/fields/SoMFPlaned.h \
    lib/database/include/Inventor/fields/SoMFRotation.h \
    lib/database/include/Inventor/fields/SoMFRotationd.h \
    lib/database/include/Inventor/fields/SoMFShort.h \
    lib/database/include/Inventor/fields/SoMFString.h \
    lib/database/include/Inventor/fields/SoMFTime.h \
    lib/database/include/Inventor/fields/SoMFUInt32.h \
    lib/database/include/Inventor/fields/SoMFULong.h \
    lib/database/include/Inventor/fields/SoMFVec2f.h \
    lib/database/include/Inventor/fields/SoMFVec2d.h \
    lib/database/include/Inventor/fields/SoMFVec3f.h \
    lib/database/include/Inventor/fields/SoMFVec3d.h \
    lib/database/include/Inventor/fields/SoMFVec4f.h \
    lib/database/include/Inventor/fields/SoMFVec4d.h \
    lib/database/include/Inventor/fields/SoSFBitMask.h \
    lib/database/include/Inventor/fields/SoSFBool.h \
    lib/database/include/Inventor/fields/SoSFColor.h \
    lib/database/include/Inventor/fields/SoSFDouble.h \
    lib/database/include/Inventor/fields/SoSFEngine.h \
    lib/database/include/Inventor/fields/SoSFEnum.h \
    lib/database/include/Inventor/fields/SoSFFloat.h \
    lib/database/include/Inventor/fields/SoSFImage.h \
    lib/database/include/Inventor/fields/SoSFLong.h \
    lib/database/include/Inventor/fields/SoSFMatrix.h \
    lib/database/include/Inventor/fields/SoSFMatrixd.h \
    lib/database/include/Inventor/fields/SoSFName.h \
    lib/database/include/Inventor/fields/SoSFNode.h \
    lib/database/include/Inventor/fields/SoSFPath.h \
    lib/database/include/Inventor/fields/SoSFPlane.h \
    lib/database/include/Inventor/fields/SoSFPlaned.h \
    lib/database/include/Inventor/fields/SoSFRotation.h \
    lib/database/include/Inventor/fields/SoSFRotationd.h \
    lib/database/include/Inventor/fields/SoSFShort.h \
    lib/database/include/Inventor/fields/SoSFString.h \
    lib/database/include/Inventor/fields/SoSFTime.h \
    lib/database/include/Inventor/fields/SoSFTrigger.h \
    lib/database/include/Inventor/fields/SoSFUInt32.h \
    lib/database/include/Inventor/fields/SoSFULong.h \
    lib/database/include/Inventor/fields/SoSFUShort.h \
    lib/database/include/Inventor/fields/SoSFVec2f.h \
    lib/database/include/Inventor/fields/SoSFVec2d.h \
    lib/database/include/Inventor/fields/SoSFVec3f.h \
    lib/database/include/Inventor/fields/SoSFVec3d.h \
    lib/database/include/Inventor/fields/SoSFVec4f.h \
    lib/database/include/Inventor/fields/SoSFVec4d.h \
    lib/database/include/Inventor/fields/SoSubField.h
  QMAKE_BUNDLE_DATA += FRAMEWORK_HEADERS12

  FRAMEWORK_HEADERS13.path = Headers/manips
  FRAMEWORK_HEADERS13.files = \
    lib/interaction/include/Inventor/manips/SoCenterballManip.h \
    lib/interaction/include/Inventor/manips/SoDirectionalLightManip.h \
    lib/interaction/include/Inventor/manips/SoHandleBoxManip.h \
    lib/interaction/include/Inventor/manips/SoJackManip.h \
    lib/interaction/include/Inventor/manips/SoPointLightManip.h \
    lib/interaction/include/Inventor/manips/SoSpotLightManip.h \
    lib/interaction/include/Inventor/manips/SoTabBoxManip.h \
    lib/interaction/include/Inventor/manips/SoTrackballManip.h \
    lib/interaction/include/Inventor/manips/SoTransformBoxManip.h \
    lib/interaction/include/Inventor/manips/SoTransformerManip.h \
    lib/interaction/include/Inventor/manips/SoTransformManip.h
  QMAKE_BUNDLE_DATA += FRAMEWORK_HEADERS13

  FRAMEWORK_HEADERS14.path = Headers/misc
  FRAMEWORK_HEADERS14.files = \
    lib/database/include/Inventor/misc/SoAuditorList.h \
    lib/database/include/Inventor/misc/SoBase.h \
    lib/database/include/Inventor/misc/SoBasic.h \
    lib/database/include/Inventor/misc/SoByteStream.h \
    lib/database/include/Inventor/misc/SoCallbackList.h \
    lib/database/include/Inventor/misc/SoChildList.h \
    lib/database/include/Inventor/misc/SoCompactPathList.h \
    lib/database/include/Inventor/misc/SoNormalGenerator.h \
    lib/database/include/Inventor/misc/SoNotification.h \
    lib/database/include/Inventor/misc/SoState.h \
    lib/database/include/Inventor/misc/SoTempPath.h \
    lib/database/include/Inventor/misc/SoTranscribe.h \
    lib/mevis/include/Inventor/misc/SoVBO.h
  QMAKE_BUNDLE_DATA += FRAMEWORK_HEADERS14

  FRAMEWORK_HEADERS15.path = Headers/misc/upgraders
  FRAMEWORK_HEADERS15.files = \
    lib/database/include/Inventor/misc/upgraders/SoV1QuadMeshKit.h \
    lib/database/include/Inventor/misc/upgraders/SoUpgrader.h \
    lib/database/include/Inventor/misc/upgraders/SoV1AppearanceKit.h \
    lib/database/include/Inventor/misc/upgraders/SoV1BaseKit.h \
    lib/database/include/Inventor/misc/upgraders/SoV1CameraKit.h \
    lib/database/include/Inventor/misc/upgraders/SoV1ConeKit.h \
    lib/database/include/Inventor/misc/upgraders/SoV1CubeKit.h \
    lib/database/include/Inventor/misc/upgraders/SoV1CustomNode.h \
    lib/database/include/Inventor/misc/upgraders/SoV1CylinderKit.h \
    lib/database/include/Inventor/misc/upgraders/SoV1DirectionalLightKit.h \
    lib/database/include/Inventor/misc/upgraders/SoV1FaceSetKit.h \
    lib/database/include/Inventor/misc/upgraders/SoV1GroupKit.h \
    lib/database/include/Inventor/misc/upgraders/SoV1IndexedFaceSetKit.h \
    lib/database/include/Inventor/misc/upgraders/SoV1IndexedLineSetKit.h \
    lib/database/include/Inventor/misc/upgraders/SoV1IndexedNurbsCurveKit.h \
    lib/database/include/Inventor/misc/upgraders/SoV1IndexedNurbsSurfaceKit.h \
    lib/database/include/Inventor/misc/upgraders/SoV1IndexedTriangleMeshKit.h \
    lib/database/include/Inventor/misc/upgraders/SoV1LightKit.h \
    lib/database/include/Inventor/misc/upgraders/SoV1LineSetKit.h \
    lib/database/include/Inventor/misc/upgraders/SoV1NodekitCatalog.h \
    lib/database/include/Inventor/misc/upgraders/SoV1NodekitParts.h \
    lib/database/include/Inventor/misc/upgraders/SoV1NurbsCurveKit.h \
    lib/database/include/Inventor/misc/upgraders/SoV1NurbsSurfaceKit.h \
    lib/database/include/Inventor/misc/upgraders/SoV1OrthographicCameraKit.h \
    lib/database/include/Inventor/misc/upgraders/SoV1PerspectiveCameraKit.h \
    lib/database/include/Inventor/misc/upgraders/SoV1PointLightKit.h \
    lib/database/include/Inventor/misc/upgraders/SoV1PointSetKit.h \
    lib/database/include/Inventor/misc/upgraders/SoV1SceneKit.h \
    lib/database/include/Inventor/misc/upgraders/SoV1ShapeKit.h \
    lib/database/include/Inventor/misc/upgraders/SoV1SphereKit.h \
    lib/database/include/Inventor/misc/upgraders/SoV1SpotLightKit.h \
    lib/database/include/Inventor/misc/upgraders/SoV1SubKit.h \
    lib/database/include/Inventor/misc/upgraders/SoV1Text2Kit.h \
    lib/database/include/Inventor/misc/upgraders/SoV1Text3Kit.h \
    lib/database/include/Inventor/misc/upgraders/SoV1TriangleStripSetKit.h \
    lib/database/include/Inventor/misc/upgraders/SoV1VertexShapeKit.h \
    lib/database/include/Inventor/misc/upgraders/SoV1WrapperKit.h
    lib/nodekits/include/Inventor/misc/upgraders/SoV1NodeKit.h \
  QMAKE_BUNDLE_DATA += FRAMEWORK_HEADERS15

  FRAMEWORK_HEADERS16.path = Headers/nodekits
  FRAMEWORK_HEADERS16.files = \
    lib/interaction/include/Inventor/nodekits/SoInteractionKit.h \
    lib/nodekits/include/Inventor/nodekits/SoAppearanceKit.h \
    lib/nodekits/include/Inventor/nodekits/SoBaseKit.h \
    lib/nodekits/include/Inventor/nodekits/SoCameraKit.h \
    lib/nodekits/include/Inventor/nodekits/SoLightKit.h \
    lib/nodekits/include/Inventor/nodekits/SoNodeKit.h \
    lib/nodekits/include/Inventor/nodekits/SoNodekitCatalog.h \
    lib/nodekits/include/Inventor/nodekits/SoNodeKitListPart.h \
    lib/nodekits/include/Inventor/nodekits/SoNodekitParts.h \
    lib/nodekits/include/Inventor/nodekits/SoSceneKit.h \
    lib/nodekits/include/Inventor/nodekits/SoSeparatorKit.h \
    lib/nodekits/include/Inventor/nodekits/SoShapeKit.h \
    lib/nodekits/include/Inventor/nodekits/SoSubKit.h \
    lib/nodekits/include/Inventor/nodekits/SoWrapperKit.h
  QMAKE_BUNDLE_DATA += FRAMEWORK_HEADERS16

  FRAMEWORK_HEADERS17.path = Headers/nodes
  FRAMEWORK_HEADERS17.files = \
    lib/mevis/include/Inventor/nodes/SoPolygonOffset.h \
    lib/database/include/Inventor/nodes/SoAnnotation.h \
    lib/database/include/Inventor/nodes/SoArray.h \
    lib/database/include/Inventor/nodes/SoAsciiText.h \
    lib/database/include/Inventor/nodes/SoBaseColor.h \
    lib/database/include/Inventor/nodes/SoBlinker.h \
    lib/database/include/Inventor/nodes/SoCallback.h \
    lib/database/include/Inventor/nodes/SoCamera.h \
    lib/database/include/Inventor/nodes/SoClipPlane.h \
    lib/database/include/Inventor/nodes/SoColorIndex.h \
    lib/database/include/Inventor/nodes/SoComplexity.h \
    lib/database/include/Inventor/nodes/SoCone.h \
    lib/database/include/Inventor/nodes/SoCoordinate3.h \
    lib/database/include/Inventor/nodes/SoCoordinate4.h \
    lib/database/include/Inventor/nodes/SoCube.h \
    lib/database/include/Inventor/nodes/SoCylinder.h \
    lib/database/include/Inventor/nodes/SoDirectionalLight.h \
    lib/database/include/Inventor/nodes/SoDrawStyle.h \
    lib/database/include/Inventor/nodes/SoEnvironment.h \
    lib/database/include/Inventor/nodes/SoEventCallback.h \
    lib/database/include/Inventor/nodes/SoFaceSet.h \
    lib/database/include/Inventor/nodes/SoFile.h \
    lib/database/include/Inventor/nodes/SoFont.h \
    lib/database/include/Inventor/nodes/SoFontStyle.h \
    lib/database/include/Inventor/nodes/SoGroup.h \
    lib/database/include/Inventor/nodes/SoIndexedFaceSet.h \
    lib/database/include/Inventor/nodes/SoIndexedTriangleSet.h \
    lib/database/include/Inventor/nodes/SoIndexedLineSet.h \
    lib/database/include/Inventor/nodes/SoIndexedNurbsCurve.h \
    lib/database/include/Inventor/nodes/SoIndexedNurbsSurface.h \
    lib/database/include/Inventor/nodes/SoIndexedShape.h \
    lib/database/include/Inventor/nodes/SoIndexedTriangleStripSet.h \
    lib/database/include/Inventor/nodes/SoInfo.h \
    lib/database/include/Inventor/nodes/SoLOD.h \
    lib/database/include/Inventor/nodes/SoLabel.h \
    lib/database/include/Inventor/nodes/SoLevelOfDetail.h \
    lib/database/include/Inventor/nodes/SoLight.h \
    lib/database/include/Inventor/nodes/SoLightModel.h \
    lib/database/include/Inventor/nodes/SoLineSet.h \
    lib/database/include/Inventor/nodes/SoLinearProfile.h \
    lib/database/include/Inventor/nodes/SoLocateHighlight.h \
    lib/database/include/Inventor/nodes/SoMaterial.h \
    lib/database/include/Inventor/nodes/SoMaterialBinding.h \
    lib/database/include/Inventor/nodes/SoMatrixTransform.h \
    lib/database/include/Inventor/nodes/SoMultipleCopy.h \
    lib/database/include/Inventor/nodes/SoNode.h \
    lib/database/include/Inventor/nodes/SoNodes.h \
    lib/database/include/Inventor/nodes/SoNonIndexedShape.h \
    lib/database/include/Inventor/nodes/SoNormal.h \
    lib/database/include/Inventor/nodes/SoNormalBinding.h \
    lib/database/include/Inventor/nodes/SoNurbsCurve.h \
    lib/database/include/Inventor/nodes/SoNurbsProfile.h \
    lib/database/include/Inventor/nodes/SoNurbsSurface.h \
    lib/database/include/Inventor/nodes/SoOrthographicCamera.h \
    lib/database/include/Inventor/nodes/SoPackedColor.h \
    lib/database/include/Inventor/nodes/SoPathSwitch.h \
    lib/database/include/Inventor/nodes/SoPendulum.h \
    lib/database/include/Inventor/nodes/SoPerspectiveCamera.h \
    lib/database/include/Inventor/nodes/SoPickStyle.h \
    lib/database/include/Inventor/nodes/SoPointLight.h \
    lib/database/include/Inventor/nodes/SoPointSet.h \
    lib/database/include/Inventor/nodes/SoProfile.h \
    lib/database/include/Inventor/nodes/SoProfileCoordinate2.h \
    lib/database/include/Inventor/nodes/SoProfileCoordinate3.h \
    lib/database/include/Inventor/nodes/SoQuadMesh.h \
    lib/database/include/Inventor/nodes/SoResetTransform.h \
    lib/database/include/Inventor/nodes/SoRotation.h \
    lib/database/include/Inventor/nodes/SoRotationXYZ.h \
    lib/database/include/Inventor/nodes/SoRotor.h \
    lib/database/include/Inventor/nodes/SoScale.h \
    lib/database/include/Inventor/nodes/SoSeparator.h \
    lib/database/include/Inventor/nodes/SoShape.h \
    lib/database/include/Inventor/nodes/SoShapeHints.h \
    lib/database/include/Inventor/nodes/SoShuttle.h \
    lib/database/include/Inventor/nodes/SoSphere.h \
    lib/database/include/Inventor/nodes/SoSpotLight.h \
    lib/database/include/Inventor/nodes/SoSubNode.h \
    lib/database/include/Inventor/nodes/SoSwitch.h \
    lib/database/include/Inventor/nodes/SoText2.h \
    lib/database/include/Inventor/nodes/SoText3.h \
    lib/database/include/Inventor/nodes/SoTexture2.h \
    lib/database/include/Inventor/nodes/SoTexture2Transform.h \
    lib/database/include/Inventor/nodes/SoTextureCoordinate2.h \
    lib/database/include/Inventor/nodes/SoTextureCoordinateBinding.h \
    lib/database/include/Inventor/nodes/SoTextureCoordinateDefault.h \
    lib/database/include/Inventor/nodes/SoTextureCoordinateEnvironment.h \
    lib/database/include/Inventor/nodes/SoTextureCoordinateFunction.h \
    lib/database/include/Inventor/nodes/SoTextureCoordinatePlane.h \
    lib/database/include/Inventor/nodes/SoTransform.h \
    lib/database/include/Inventor/nodes/SoTransformSeparator.h \
    lib/database/include/Inventor/nodes/SoTransformation.h \
    lib/database/include/Inventor/nodes/SoTranslation.h \
    lib/database/include/Inventor/nodes/SoTriangleStripSet.h \
    lib/database/include/Inventor/nodes/SoUnits.h \
    lib/database/include/Inventor/nodes/SoVertexProperty.h \
    lib/database/include/Inventor/nodes/SoVertexShape.h \
    lib/database/include/Inventor/nodes/SoWWWAnchor.h \
    lib/database/include/Inventor/nodes/SoWWWInline.h \
    lib/interaction/include/Inventor/nodes/SoAntiSquish.h \
    lib/interaction/include/Inventor/nodes/SoSelection.h \
    lib/interaction/include/Inventor/nodes/SoSurroundScale.h
  QMAKE_BUNDLE_DATA += FRAMEWORK_HEADERS17

  FRAMEWORK_HEADERS18.path = Headers/projectors
  FRAMEWORK_HEADERS18.files = \
    lib/database/include/Inventor/projectors/SbCylinderPlaneProjector.h \
    lib/database/include/Inventor/projectors/SbCylinderProjector.h \
    lib/database/include/Inventor/projectors/SbCylinderSectionProjector.h \
    lib/database/include/Inventor/projectors/SbCylinderSheetProjector.h \
    lib/database/include/Inventor/projectors/SbLineProjector.h \
    lib/database/include/Inventor/projectors/SbPlaneProjector.h \
    lib/database/include/Inventor/projectors/SbProjector.h \
    lib/database/include/Inventor/projectors/SbProjectors.h \
    lib/database/include/Inventor/projectors/SbSpherePlaneProjector.h \
    lib/database/include/Inventor/projectors/SbSphereProjector.h \
    lib/database/include/Inventor/projectors/SbSphereSectionProjector.h \
    lib/database/include/Inventor/projectors/SbSphereSheetProjector.h
  QMAKE_BUNDLE_DATA += FRAMEWORK_HEADERS18

  FRAMEWORK_HEADERS19.path = Headers/sensors
  FRAMEWORK_HEADERS19.files = \
    lib/database/include/Inventor/sensors/SoAlarmSensor.h \
    lib/database/include/Inventor/sensors/SoDataSensor.h \
    lib/database/include/Inventor/sensors/SoDelayQueueSensor.h \
    lib/database/include/Inventor/sensors/SoFieldSensor.h \
    lib/database/include/Inventor/sensors/SoIdleSensor.h \
    lib/database/include/Inventor/sensors/SoNodeSensor.h \
    lib/database/include/Inventor/sensors/SoOneShotSensor.h \
    lib/database/include/Inventor/sensors/SoPathSensor.h \
    lib/database/include/Inventor/sensors/SoSensor.h \
    lib/database/include/Inventor/sensors/SoSensorManager.h \
    lib/database/include/Inventor/sensors/SoSensors.h \
    lib/database/include/Inventor/sensors/SoTimerQueueSensor.h \
    lib/database/include/Inventor/sensors/SoTimerSensor.h
  QMAKE_BUNDLE_DATA += FRAMEWORK_HEADERS19

  FRAMEWORK_HEADERS20.path = Headers/system
  FRAMEWORK_HEADERS20.files = \
    lib/mevis/include/Inventor/system/SbConfig.h \
    lib/mevis/include/Inventor/system/SbSystem.h \ 
    lib/mevis/include/Inventor/system/SbOpenGL.h \
    lib/mevis/include/Inventor/system/SbOpenGLU.h
  QMAKE_BUNDLE_DATA += FRAMEWORK_HEADERS20

  build_oiv_installer {
    QMAKE_LFLAGS_SONAME = -Wl,-install_name,/Library/Frameworks/
  }
}

