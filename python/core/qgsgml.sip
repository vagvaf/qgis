/************************************************************************
 * This file has been generated automatically from                      *
 *                                                                      *
 * src/core/qgsgml.h                                                    *
 *                                                                      *
 * Do not edit manually ! Edit header and run scripts/sipify.pl again   *
 ************************************************************************/






class QgsGml : QObject
{
%Docstring
 This class reads data from a WFS server or alternatively from a GML file. It
 uses the expat XML parser and an event based model to keep performance high.
 The parsing starts when the first data arrives, it does not wait until the
 request is finished *
%End

%TypeHeaderCode
#include "qgsgml.h"
%End
  public:
    QgsGml(
      const QString &typeName,
      const QString &geometryAttribute,
      const QgsFields &fields );

    int getFeatures( const QString &uri,
                     QgsWkbTypes::Type *wkbType,
                     QgsRectangle *extent = 0,
                     const QString &userName = QString(),
                     const QString &password = QString(),
                     const QString &authcfg = QString() ) /PyName=getFeaturesUri/;
%Docstring
 Does the Http GET request to the wfs server
  Supports only UTF-8, UTF-16, ISO-8859-1, ISO-8859-1 XML encodings.
  \param uri GML URL
  \param wkbType wkbType to retrieve
  \param extent retrieved extents
  \param userName username for authentication
  \param password password for authentication
  \param authcfg authentication configuration id
  :return: 0 in case of success
.. note::

   available in Python as getFeaturesUri
 :rtype: int
%End

    int getFeatures( const QByteArray &data, QgsWkbTypes::Type *wkbType, QgsRectangle *extent = 0 );
%Docstring
 Read from GML data. Constructor uri param is ignored
  Supports only UTF-8, UTF-16, ISO-8859-1, ISO-8859-1 XML encodings.
 :rtype: int
%End

    QMap<QgsFeatureId, QgsFeature * > featuresMap() const;
%Docstring
Get parsed features for given type name
 :rtype: QMap<QgsFeatureId, QgsFeature * >
%End

    QMap<QgsFeatureId, QString > idsMap() const;
%Docstring
Get feature ids map
 :rtype: QMap<QgsFeatureId, str >
%End

    QgsCoordinateReferenceSystem crs() const;
%Docstring
 Returns features spatial reference system
.. versionadded:: 2.1
 :rtype: QgsCoordinateReferenceSystem
%End

  signals:
    void dataReadProgress( int progress );
    void totalStepsUpdate( int totalSteps );
    void dataProgressAndSteps( int progress, int totalSteps );
%Docstring
Also emit signal with progress and totalSteps together (this is better for the status message)
%End

};

/************************************************************************
 * This file has been generated automatically from                      *
 *                                                                      *
 * src/core/qgsgml.h                                                    *
 *                                                                      *
 * Do not edit manually ! Edit header and run scripts/sipify.pl again   *
 ************************************************************************/
