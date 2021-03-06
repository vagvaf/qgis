/***************************************************************************
    qgspainteffectwidget.cpp
    ------------------------
    begin                : January 2015
    copyright            : (C) 2015 by Nyall Dawson
    email                : nyall dot dawson at gmail.com
 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


#include "qgspainteffectwidget.h"
#include "qgslogger.h"
#include "qgspainteffect.h"
#include "qgsshadoweffect.h"
#include "qgsblureffect.h"
#include "qgsgloweffect.h"
#include "qgstransformeffect.h"
#include "qgscoloreffect.h"
#include "qgsstyle.h"
#include "qgscolorramp.h"
#include "qgscolorrampbutton.h"

//
// draw source
//

QgsDrawSourceWidget::QgsDrawSourceWidget( QWidget *parent )
  : QgsPaintEffectWidget( parent )
  , mEffect( nullptr )
{
  setupUi( this );
  initGui();
}


void QgsDrawSourceWidget::setPaintEffect( QgsPaintEffect *effect )
{
  if ( !effect || effect->type() != QLatin1String( "drawSource" ) )
    return;

  mEffect = static_cast<QgsDrawSourceEffect *>( effect );
  initGui();

  connect( mOpacityWidget, &QgsOpacityWidget::opacityChanged, this, &QgsDrawSourceWidget::opacityChanged );
}

void QgsDrawSourceWidget::initGui()
{
  if ( !mEffect )
  {
    return;
  }

  blockSignals( true );

  mOpacityWidget->setOpacity( mEffect->opacity() );
  mBlendCmbBx->setBlendMode( mEffect->blendMode() );
  mDrawModeComboBox->setDrawMode( mEffect->drawMode() );

  blockSignals( false );
}

void QgsDrawSourceWidget::blockSignals( const bool block )
{
  mOpacityWidget->blockSignals( block );
  mBlendCmbBx->blockSignals( block );
  mDrawModeComboBox->blockSignals( block );
}

void QgsDrawSourceWidget::opacityChanged( double value )
{
  if ( !mEffect )
    return;

  mEffect->setOpacity( value );
  emit changed();
}

void QgsDrawSourceWidget::on_mDrawModeComboBox_currentIndexChanged( int index )
{
  Q_UNUSED( index );

  if ( !mEffect )
    return;

  mEffect->setDrawMode( mDrawModeComboBox->drawMode() );
  emit changed();
}

void QgsDrawSourceWidget::on_mBlendCmbBx_currentIndexChanged( int index )
{
  Q_UNUSED( index );

  if ( !mEffect )
    return;

  mEffect->setBlendMode( mBlendCmbBx->blendMode() );
  emit changed();
}


//
// blur
//

QgsBlurWidget::QgsBlurWidget( QWidget *parent )
  : QgsPaintEffectWidget( parent )
  , mEffect( nullptr )
{
  setupUi( this );

  mBlurTypeCombo->addItem( tr( "Stack blur (fast)" ), QgsBlurEffect::StackBlur );
  mBlurTypeCombo->addItem( tr( "Gaussian blur (quality)" ), QgsBlurEffect::GaussianBlur );

  initGui();
  connect( mOpacityWidget, &QgsOpacityWidget::opacityChanged, this, &QgsBlurWidget::opacityChanged );
}


void QgsBlurWidget::setPaintEffect( QgsPaintEffect *effect )
{
  if ( !effect || effect->type() != QLatin1String( "blur" ) )
    return;

  mEffect = static_cast<QgsBlurEffect *>( effect );
  initGui();
}

void QgsBlurWidget::initGui()
{
  if ( !mEffect )
  {
    return;
  }

  blockSignals( true );

  mBlurTypeCombo->setCurrentIndex( mBlurTypeCombo->findData( mEffect->blurMethod() ) );
  mBlurStrengthSpnBx->setValue( mEffect->blurLevel() );
  mOpacityWidget->setOpacity( mEffect->opacity() );
  mBlendCmbBx->setBlendMode( mEffect->blendMode() );
  mDrawModeComboBox->setDrawMode( mEffect->drawMode() );

  blockSignals( false );
}

void QgsBlurWidget::blockSignals( const bool block )
{
  mBlurTypeCombo->blockSignals( block );
  mBlurStrengthSpnBx->blockSignals( block );
  mOpacityWidget->blockSignals( block );
  mBlendCmbBx->blockSignals( block );
  mDrawModeComboBox->blockSignals( block );
}

void QgsBlurWidget::on_mBlurTypeCombo_currentIndexChanged( int index )
{
  if ( !mEffect )
    return;

  QgsBlurEffect::BlurMethod method = ( QgsBlurEffect::BlurMethod ) mBlurTypeCombo->itemData( index ).toInt();
  mEffect->setBlurMethod( method );

  //also update max radius
  switch ( method )
  {
    case QgsBlurEffect::StackBlur:
      mBlurStrengthSpnBx->setMaximum( 16 );
      break;
    case QgsBlurEffect::GaussianBlur:
      mBlurStrengthSpnBx->setMaximum( 200 );
      break;
  }

  emit changed();
}

void QgsBlurWidget::on_mBlurStrengthSpnBx_valueChanged( int value )
{
  if ( !mEffect )
    return;

  mEffect->setBlurLevel( value );
  emit changed();
}

void QgsBlurWidget::opacityChanged( double value )
{
  if ( !mEffect )
    return;

  mEffect->setOpacity( value );
  emit changed();
}

void QgsBlurWidget::on_mDrawModeComboBox_currentIndexChanged( int index )
{
  Q_UNUSED( index );

  if ( !mEffect )
    return;

  mEffect->setDrawMode( mDrawModeComboBox->drawMode() );
  emit changed();
}

void QgsBlurWidget::on_mBlendCmbBx_currentIndexChanged( int index )
{
  Q_UNUSED( index );

  if ( !mEffect )
    return;

  mEffect->setBlendMode( mBlendCmbBx->blendMode() );
  emit changed();
}


//
// Drop Shadow
//

QgsShadowEffectWidget::QgsShadowEffectWidget( QWidget *parent )
  : QgsPaintEffectWidget( parent )
  , mEffect( nullptr )
{
  setupUi( this );

  mShadowColorBtn->setAllowOpacity( false );
  mShadowColorBtn->setColorDialogTitle( tr( "Select shadow color" ) );
  mShadowColorBtn->setContext( QStringLiteral( "symbology" ) );
  mShadowOffsetAngleSpnBx->setClearValue( 0 );

  mOffsetUnitWidget->setUnits( QgsUnitTypes::RenderUnitList() << QgsUnitTypes::RenderMillimeters << QgsUnitTypes::RenderPixels << QgsUnitTypes::RenderMapUnits
                               << QgsUnitTypes::RenderPoints << QgsUnitTypes::RenderInches );

  initGui();

  connect( mOpacityWidget, &QgsOpacityWidget::opacityChanged, this, &QgsShadowEffectWidget::opacityChanged );
}

void QgsShadowEffectWidget::setPaintEffect( QgsPaintEffect *effect )
{
  if ( !effect || ( effect->type() != QLatin1String( "dropShadow" ) && effect->type() != QLatin1String( "innerShadow" ) ) )
    return;

  mEffect = static_cast<QgsShadowEffect *>( effect );
  initGui();
}

void QgsShadowEffectWidget::initGui()
{
  if ( !mEffect )
  {
    return;
  }

  blockSignals( true );

  mShadowOffsetAngleSpnBx->setValue( mEffect->offsetAngle() );
  mShadowOffsetAngleDial->setValue( mEffect->offsetAngle() );
  mShadowOffsetSpnBx->setValue( mEffect->offsetDistance() );
  mOffsetUnitWidget->setUnit( mEffect->offsetUnit() );
  mOffsetUnitWidget->setMapUnitScale( mEffect->offsetMapUnitScale() );
  mShadowRadiuSpnBx->setValue( mEffect->blurLevel() );
  mOpacityWidget->setOpacity( mEffect->opacity() );
  mShadowColorBtn->setColor( mEffect->color() );
  mShadowBlendCmbBx->setBlendMode( mEffect->blendMode() );
  mDrawModeComboBox->setDrawMode( mEffect->drawMode() );

  blockSignals( false );
}

void QgsShadowEffectWidget::blockSignals( const bool block )
{
  mShadowOffsetAngleSpnBx->blockSignals( block );
  mShadowOffsetAngleDial->blockSignals( block );
  mShadowOffsetSpnBx->blockSignals( block );
  mOffsetUnitWidget->blockSignals( block );
  mShadowRadiuSpnBx->blockSignals( block );
  mOpacityWidget->blockSignals( block );
  mShadowColorBtn->blockSignals( block );
  mShadowBlendCmbBx->blockSignals( block );
  mDrawModeComboBox->blockSignals( block );
}

void QgsShadowEffectWidget::on_mShadowOffsetAngleSpnBx_valueChanged( int value )
{
  mShadowOffsetAngleDial->blockSignals( true );
  mShadowOffsetAngleDial->setValue( value );
  mShadowOffsetAngleDial->blockSignals( false );

  if ( !mEffect )
    return;

  mEffect->setOffsetAngle( value );
  emit changed();
}

void QgsShadowEffectWidget::on_mShadowOffsetAngleDial_valueChanged( int value )
{
  mShadowOffsetAngleSpnBx->setValue( value );
}

void QgsShadowEffectWidget::on_mShadowOffsetSpnBx_valueChanged( double value )
{
  if ( !mEffect )
    return;

  mEffect->setOffsetDistance( value );
  emit changed();
}

void QgsShadowEffectWidget::on_mOffsetUnitWidget_changed()
{
  if ( !mEffect )
  {
    return;
  }

  mEffect->setOffsetUnit( mOffsetUnitWidget->unit() );
  mEffect->setOffsetMapUnitScale( mOffsetUnitWidget->getMapUnitScale() );
  emit changed();
}

void QgsShadowEffectWidget::opacityChanged( double value )
{
  if ( !mEffect )
    return;

  mEffect->setOpacity( value );
  emit changed();
}

void QgsShadowEffectWidget::on_mShadowColorBtn_colorChanged( const QColor &color )
{
  if ( !mEffect )
    return;

  mEffect->setColor( color );
  emit changed();
}

void QgsShadowEffectWidget::on_mShadowRadiuSpnBx_valueChanged( int value )
{
  if ( !mEffect )
    return;

  mEffect->setBlurLevel( value );
  emit changed();
}

void QgsShadowEffectWidget::on_mDrawModeComboBox_currentIndexChanged( int index )
{
  Q_UNUSED( index );

  if ( !mEffect )
    return;

  mEffect->setDrawMode( mDrawModeComboBox->drawMode() );
  emit changed();
}

void QgsShadowEffectWidget::on_mShadowBlendCmbBx_currentIndexChanged( int index )
{
  Q_UNUSED( index );

  if ( !mEffect )
    return;

  mEffect->setBlendMode( mShadowBlendCmbBx->blendMode() );
  emit changed();
}



//
// glow
//

QgsGlowWidget::QgsGlowWidget( QWidget *parent )
  : QgsPaintEffectWidget( parent )
  , mEffect( nullptr )
{
  setupUi( this );

  mColorBtn->setAllowOpacity( false );
  mColorBtn->setColorDialogTitle( tr( "Select glow color" ) );
  mColorBtn->setContext( QStringLiteral( "symbology" ) );

  mSpreadUnitWidget->setUnits( QgsUnitTypes::RenderUnitList() << QgsUnitTypes::RenderMillimeters << QgsUnitTypes::RenderPixels << QgsUnitTypes::RenderMapUnits
                               << QgsUnitTypes::RenderPoints << QgsUnitTypes::RenderInches );

  btnColorRamp->setShowGradientOnly( true );

  initGui();

  connect( btnColorRamp, &QgsColorRampButton::colorRampChanged, this, &QgsGlowWidget::applyColorRamp );
  connect( radioSingleColor, &QAbstractButton::toggled, this, &QgsGlowWidget::colorModeChanged );
  connect( mOpacityWidget, &QgsOpacityWidget::opacityChanged, this, &QgsGlowWidget::opacityChanged );
}

void QgsGlowWidget::setPaintEffect( QgsPaintEffect *effect )
{
  if ( !effect || ( effect->type() != QLatin1String( "outerGlow" ) && effect->type() != QLatin1String( "innerGlow" ) ) )
    return;

  mEffect = static_cast<QgsGlowEffect *>( effect );
  initGui();
}

void QgsGlowWidget::initGui()
{
  if ( !mEffect )
  {
    return;
  }

  blockSignals( true );

  mSpreadSpnBx->setValue( mEffect->spread() );
  mSpreadUnitWidget->setUnit( mEffect->spreadUnit() );
  mSpreadUnitWidget->setMapUnitScale( mEffect->spreadMapUnitScale() );
  mBlurRadiusSpnBx->setValue( mEffect->blurLevel() );
  mOpacityWidget->setOpacity( mEffect->opacity() );
  mColorBtn->setColor( mEffect->color() );
  mBlendCmbBx->setBlendMode( mEffect->blendMode() );

  if ( mEffect->ramp() )
  {
    btnColorRamp->setColorRamp( mEffect->ramp() );
  }

  radioSingleColor->setChecked( mEffect->colorType() == QgsGlowEffect::SingleColor );
  mColorBtn->setEnabled( mEffect->colorType() == QgsGlowEffect::SingleColor );
  radioColorRamp->setChecked( mEffect->colorType() == QgsGlowEffect::ColorRamp );
  btnColorRamp->setEnabled( mEffect->colorType() == QgsGlowEffect::ColorRamp );
  mDrawModeComboBox->setDrawMode( mEffect->drawMode() );

  blockSignals( false );
}

void QgsGlowWidget::blockSignals( const bool block )
{
  mSpreadSpnBx->blockSignals( block );
  mSpreadUnitWidget->blockSignals( block );
  mBlurRadiusSpnBx->blockSignals( block );
  mOpacityWidget->blockSignals( block );
  mColorBtn->blockSignals( block );
  mBlendCmbBx->blockSignals( block );
  btnColorRamp->blockSignals( block );
  radioSingleColor->blockSignals( block );
  radioColorRamp->blockSignals( block );
  mDrawModeComboBox->blockSignals( block );
}

void QgsGlowWidget::colorModeChanged()
{
  if ( !mEffect )
  {
    return;
  }

  if ( radioSingleColor->isChecked() )
  {
    mEffect->setColorType( QgsGlowEffect::SingleColor );
  }
  else
  {
    mEffect->setColorType( QgsGlowEffect::ColorRamp );
    mEffect->setRamp( btnColorRamp->colorRamp() );
  }
  emit changed();
}

void QgsGlowWidget::on_mSpreadSpnBx_valueChanged( double value )
{
  if ( !mEffect )
    return;

  mEffect->setSpread( value );
  emit changed();
}

void QgsGlowWidget::on_mSpreadUnitWidget_changed()
{
  if ( !mEffect )
  {
    return;
  }

  mEffect->setSpreadUnit( mSpreadUnitWidget->unit() );
  mEffect->setSpreadMapUnitScale( mSpreadUnitWidget->getMapUnitScale() );
  emit changed();
}

void QgsGlowWidget::opacityChanged( double value )
{
  if ( !mEffect )
    return;

  mEffect->setOpacity( value );
  emit changed();
}

void QgsGlowWidget::on_mColorBtn_colorChanged( const QColor &color )
{
  if ( !mEffect )
    return;

  mEffect->setColor( color );
  emit changed();
}

void QgsGlowWidget::on_mBlurRadiusSpnBx_valueChanged( int value )
{
  if ( !mEffect )
    return;

  mEffect->setBlurLevel( value );
  emit changed();
}

void QgsGlowWidget::on_mBlendCmbBx_currentIndexChanged( int index )
{
  Q_UNUSED( index );

  if ( !mEffect )
    return;

  mEffect->setBlendMode( mBlendCmbBx->blendMode() );
  emit changed();
}

void QgsGlowWidget::on_mDrawModeComboBox_currentIndexChanged( int index )
{
  Q_UNUSED( index );

  if ( !mEffect )
    return;

  mEffect->setDrawMode( mDrawModeComboBox->drawMode() );
  emit changed();
}

void QgsGlowWidget::applyColorRamp()
{
  if ( !mEffect )
  {
    return;
  }

  QgsColorRamp *ramp = btnColorRamp->colorRamp();
  if ( !ramp )
    return;

  mEffect->setRamp( ramp );
  emit changed();
}

//
// transform
//

QgsTransformWidget::QgsTransformWidget( QWidget *parent )
  : QgsPaintEffectWidget( parent )
  , mEffect( nullptr )
{
  setupUi( this );

  mTranslateUnitWidget->setUnits( QgsUnitTypes::RenderUnitList() << QgsUnitTypes::RenderMillimeters << QgsUnitTypes::RenderPixels << QgsUnitTypes::RenderMapUnits
                                  << QgsUnitTypes::RenderPoints << QgsUnitTypes::RenderInches );
  mSpinTranslateX->setClearValue( 0 );
  mSpinTranslateY->setClearValue( 0 );
  mRotationSpinBox->setClearValue( 0 );
  mSpinShearX->setClearValue( 0 );
  mSpinShearY->setClearValue( 0 );
  mSpinScaleX->setClearValue( 100.0 );
  mSpinScaleY->setClearValue( 100.0 );

  initGui();
}


void QgsTransformWidget::setPaintEffect( QgsPaintEffect *effect )
{
  if ( !effect || effect->type() != QLatin1String( "transform" ) )
    return;

  mEffect = static_cast<QgsTransformEffect *>( effect );
  initGui();
}

void QgsTransformWidget::initGui()
{
  if ( !mEffect )
  {
    return;
  }

  blockSignals( true );

  mReflectXCheckBox->setChecked( mEffect->reflectX() );
  mReflectYCheckBox->setChecked( mEffect->reflectY() );
  mDrawModeComboBox->setDrawMode( mEffect->drawMode() );
  mSpinTranslateX->setValue( mEffect->translateX() );
  mSpinTranslateY->setValue( mEffect->translateY() );
  mTranslateUnitWidget->setUnit( mEffect->translateUnit() );
  mTranslateUnitWidget->setMapUnitScale( mEffect->translateMapUnitScale() );
  mSpinShearX->setValue( mEffect->shearX() );
  mSpinShearY->setValue( mEffect->shearY() );
  mSpinScaleX->setValue( mEffect->scaleX() * 100.0 );
  mSpinScaleY->setValue( mEffect->scaleY() * 100.0 );
  mRotationSpinBox->setValue( mEffect->rotation() );

  blockSignals( false );
}

void QgsTransformWidget::blockSignals( const bool block )
{
  mDrawModeComboBox->blockSignals( block );
  mTranslateUnitWidget->blockSignals( block );
  mSpinTranslateX->blockSignals( block );
  mSpinTranslateY->blockSignals( block );
  mReflectXCheckBox->blockSignals( block );
  mReflectYCheckBox->blockSignals( block );
  mSpinShearX->blockSignals( block );
  mSpinShearY->blockSignals( block );
  mSpinScaleX->blockSignals( block );
  mSpinScaleY->blockSignals( block );
  mRotationSpinBox->blockSignals( block );
}


void QgsTransformWidget::on_mDrawModeComboBox_currentIndexChanged( int index )
{
  Q_UNUSED( index );

  if ( !mEffect )
    return;

  mEffect->setDrawMode( mDrawModeComboBox->drawMode() );
  emit changed();
}

void QgsTransformWidget::on_mSpinTranslateX_valueChanged( double value )
{
  if ( !mEffect )
    return;

  mEffect->setTranslateX( value );
  emit changed();
}

void QgsTransformWidget::on_mSpinTranslateY_valueChanged( double value )
{
  if ( !mEffect )
    return;

  mEffect->setTranslateY( value );
  emit changed();
}

void QgsTransformWidget::on_mTranslateUnitWidget_changed()
{
  if ( !mEffect )
  {
    return;
  }

  mEffect->setTranslateUnit( mTranslateUnitWidget->unit() );
  mEffect->setTranslateMapUnitScale( mTranslateUnitWidget->getMapUnitScale() );
  emit changed();
}

void QgsTransformWidget::on_mReflectXCheckBox_stateChanged( int state )
{
  if ( !mEffect )
    return;

  mEffect->setReflectX( state == Qt::Checked );
  emit changed();
}

void QgsTransformWidget::on_mReflectYCheckBox_stateChanged( int state )
{
  if ( !mEffect )
    return;

  mEffect->setReflectY( state == Qt::Checked );
  emit changed();
}

void QgsTransformWidget::on_mSpinShearX_valueChanged( double value )
{
  if ( !mEffect )
    return;

  mEffect->setShearX( value );
  emit changed();
}

void QgsTransformWidget::on_mSpinShearY_valueChanged( double value )
{
  if ( !mEffect )
    return;

  mEffect->setShearY( value );
  emit changed();
}

void QgsTransformWidget::on_mSpinScaleX_valueChanged( double value )
{
  if ( !mEffect )
    return;

  mEffect->setScaleX( value / 100.0 );
  emit changed();
}

void QgsTransformWidget::on_mSpinScaleY_valueChanged( double value )
{
  if ( !mEffect )
    return;

  mEffect->setScaleY( value / 100.0 );
  emit changed();
}

void QgsTransformWidget::on_mRotationSpinBox_valueChanged( double value )
{
  if ( !mEffect )
    return;

  mEffect->setRotation( value );
  emit changed();
}


//
// color effect
//

QgsColorEffectWidget::QgsColorEffectWidget( QWidget *parent )
  : QgsPaintEffectWidget( parent )
  , mEffect( nullptr )
{
  setupUi( this );

  mBrightnessSpinBox->setClearValue( 0 );
  mContrastSpinBox->setClearValue( 0 );
  mSaturationSpinBox->setClearValue( 0 );
  mColorizeColorButton->setAllowOpacity( false );

  mGrayscaleCombo->addItem( tr( "Off" ), QgsImageOperation::GrayscaleOff );
  mGrayscaleCombo->addItem( tr( "By lightness" ), QgsImageOperation::GrayscaleLightness );
  mGrayscaleCombo->addItem( tr( "By luminosity" ), QgsImageOperation::GrayscaleLuminosity );
  mGrayscaleCombo->addItem( tr( "By average" ), QgsImageOperation::GrayscaleAverage );

  initGui();

  connect( mOpacityWidget, &QgsOpacityWidget::opacityChanged, this, &QgsColorEffectWidget::opacityChanged );
}

void QgsColorEffectWidget::setPaintEffect( QgsPaintEffect *effect )
{
  if ( !effect || effect->type() != QLatin1String( "color" ) )
    return;

  mEffect = static_cast<QgsColorEffect *>( effect );
  initGui();
}

void QgsColorEffectWidget::initGui()
{
  if ( !mEffect )
  {
    return;
  }

  blockSignals( true );

  mSliderBrightness->setValue( mEffect->brightness() );
  mSliderContrast->setValue( mEffect->contrast() );
  mSliderSaturation->setValue( ( mEffect->saturation() - 1.0 ) * 100.0 );
  mColorizeCheck->setChecked( mEffect->colorizeOn() );
  mSliderColorizeStrength->setValue( mEffect->colorizeStrength() );
  mColorizeColorButton->setColor( mEffect->colorizeColor() );
  int grayscaleIdx = mGrayscaleCombo->findData( QVariant( ( int ) mEffect->grayscaleMode() ) );
  mGrayscaleCombo->setCurrentIndex( grayscaleIdx == -1 ? 0 : grayscaleIdx );
  mOpacityWidget->setOpacity( mEffect->opacity() );
  mBlendCmbBx->setBlendMode( mEffect->blendMode() );
  mDrawModeComboBox->setDrawMode( mEffect->drawMode() );
  enableColorizeControls( mEffect->colorizeOn() );

  blockSignals( false );
}

void QgsColorEffectWidget::blockSignals( const bool block )
{
  mBrightnessSpinBox->blockSignals( block );
  mContrastSpinBox->blockSignals( block );
  mSaturationSpinBox->blockSignals( block );
  mColorizeStrengthSpinBox->blockSignals( block );
  mColorizeCheck->blockSignals( block );
  mColorizeColorButton->blockSignals( block );
  mGrayscaleCombo->blockSignals( block );
  mOpacityWidget->blockSignals( block );
  mBlendCmbBx->blockSignals( block );
  mDrawModeComboBox->blockSignals( block );
}

void QgsColorEffectWidget::enableColorizeControls( const bool enable )
{
  mSliderColorizeStrength->setEnabled( enable );
  mColorizeStrengthSpinBox->setEnabled( enable );
  mColorizeColorButton->setEnabled( enable );
}

void QgsColorEffectWidget::opacityChanged( double value )
{
  if ( !mEffect )
    return;

  mEffect->setOpacity( value );
  emit changed();
}

void QgsColorEffectWidget::on_mBlendCmbBx_currentIndexChanged( int index )
{
  Q_UNUSED( index );

  if ( !mEffect )
    return;

  mEffect->setBlendMode( mBlendCmbBx->blendMode() );
  emit changed();
}

void QgsColorEffectWidget::on_mDrawModeComboBox_currentIndexChanged( int index )
{
  Q_UNUSED( index );

  if ( !mEffect )
    return;

  mEffect->setDrawMode( mDrawModeComboBox->drawMode() );
  emit changed();
}

void QgsColorEffectWidget::on_mBrightnessSpinBox_valueChanged( int value )
{
  if ( !mEffect )
    return;

  mEffect->setBrightness( value );
  emit changed();
}

void QgsColorEffectWidget::on_mContrastSpinBox_valueChanged( int value )
{
  if ( !mEffect )
    return;

  mEffect->setContrast( value );
  emit changed();
}

void QgsColorEffectWidget::on_mSaturationSpinBox_valueChanged( int value )
{
  if ( !mEffect )
    return;

  mEffect->setSaturation( value / 100.0 + 1 );
  emit changed();
}

void QgsColorEffectWidget::on_mColorizeStrengthSpinBox_valueChanged( int value )
{
  if ( !mEffect )
    return;

  mEffect->setColorizeStrength( value );
  emit changed();
}

void QgsColorEffectWidget::on_mColorizeCheck_stateChanged( int state )
{
  if ( !mEffect )
    return;

  mEffect->setColorizeOn( state == Qt::Checked );
  enableColorizeControls( state == Qt::Checked );
  emit changed();
}

void QgsColorEffectWidget::on_mColorizeColorButton_colorChanged( const QColor &color )
{
  if ( !mEffect )
    return;

  mEffect->setColorizeColor( color );
  emit changed();
}

void QgsColorEffectWidget::on_mGrayscaleCombo_currentIndexChanged( int index )
{
  Q_UNUSED( index );

  if ( !mEffect )
    return;

  mEffect->setGrayscaleMode( ( QgsImageOperation::GrayscaleMode ) mGrayscaleCombo->currentData().toInt() );
  emit changed();
}
