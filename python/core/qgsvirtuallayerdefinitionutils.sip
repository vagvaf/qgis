/************************************************************************
 * This file has been generated automatically from                      *
 *                                                                      *
 * src/core/qgsvirtuallayerdefinitionutils.h                            *
 *                                                                      *
 * Do not edit manually ! Edit header and run scripts/sipify.pl again   *
 ************************************************************************/





class QgsVirtualLayerDefinitionUtils
{
%Docstring
 Utils class for QgsVirtualLayerDefinition
%End

%TypeHeaderCode
#include "qgsvirtuallayerdefinitionutils.h"
%End
  public:
    static QgsVirtualLayerDefinition fromJoinedLayer( QgsVectorLayer *joinedLayer );
%Docstring
Get a virtual layer definition from a vector layer where vector joins are replaced by SQL LEFT JOINs
 :rtype: QgsVirtualLayerDefinition
%End
};

/************************************************************************
 * This file has been generated automatically from                      *
 *                                                                      *
 * src/core/qgsvirtuallayerdefinitionutils.h                            *
 *                                                                      *
 * Do not edit manually ! Edit header and run scripts/sipify.pl again   *
 ************************************************************************/
