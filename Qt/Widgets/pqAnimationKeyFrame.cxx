// SPDX-FileCopyrightText: Copyright (c) Kitware Inc.
// SPDX-FileCopyrightText: Copyright (c) Sandia Corporation
// SPDX-License-Identifier: BSD-3-Clause

#include "pqAnimationKeyFrame.h"

#include "pqAnimationTrack.h"
#include <QFontMetrics>
#include <QGraphicsView>
#include <QPainter>
#include <QWidget>

//-----------------------------------------------------------------------------
pqAnimationKeyFrame::pqAnimationKeyFrame(pqAnimationTrack* p)
  : QObject(p)
  , QGraphicsItem(p)
  , Rect(0, 0, 1, 1)
{
}

//-----------------------------------------------------------------------------
pqAnimationTrack* pqAnimationKeyFrame::parentTrack() const
{
  return qobject_cast<pqAnimationTrack*>(this->QObject::parent());
}

//-----------------------------------------------------------------------------
QVariant pqAnimationKeyFrame::startValue() const
{
  return this->StartValue;
}

//-----------------------------------------------------------------------------
QVariant pqAnimationKeyFrame::endValue() const
{
  return this->EndValue;
}

//-----------------------------------------------------------------------------
QIcon pqAnimationKeyFrame::icon() const
{
  return this->Icon;
}

//-----------------------------------------------------------------------------
double pqAnimationKeyFrame::normalizedStartTime() const
{
  return this->NormalizedStartTime;
}

//-----------------------------------------------------------------------------
double pqAnimationKeyFrame::normalizedEndTime() const
{
  return this->NormalizedEndTime;
}

//-----------------------------------------------------------------------------
void pqAnimationKeyFrame::setNormalizedStartTime(double t)
{
  this->NormalizedStartTime = t;
}

//-----------------------------------------------------------------------------
void pqAnimationKeyFrame::setNormalizedEndTime(double t)
{
  this->NormalizedEndTime = t;
}

//-----------------------------------------------------------------------------
void pqAnimationKeyFrame::setStartValue(const QVariant& v)
{
  this->StartValue = v;
  this->update();
}

//-----------------------------------------------------------------------------
void pqAnimationKeyFrame::setEndValue(const QVariant& v)
{
  this->EndValue = v;
  this->update();
}

//-----------------------------------------------------------------------------
void pqAnimationKeyFrame::setIcon(const QIcon& i)
{
  this->Icon = i;
  this->update();
}

//-----------------------------------------------------------------------------
void pqAnimationKeyFrame::adjustRect(double startPos, double endPos)
{
  pqAnimationTrack* track = qobject_cast<pqAnimationTrack*>(this->parent());
  QRectF trackRect = track->boundingRect();

  this->setBoundingRect(QRectF(startPos, trackRect.top(), endPos - startPos, trackRect.height()));
}

//-----------------------------------------------------------------------------
QRectF pqAnimationKeyFrame::boundingRect() const
{
  return this->Rect;
}

//-----------------------------------------------------------------------------
void pqAnimationKeyFrame::setBoundingRect(const QRectF& r)
{
  this->removeFromIndex();
  this->Rect = r;
  this->addToIndex();
  this->update();
}

//-----------------------------------------------------------------------------
void pqAnimationKeyFrame::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget* widget)
{
  painter->save();
  if (this->parentTrack()->isEnabled())
  {
    // change brush only when parent track is enabled.
    painter->setBrush(widget->palette().base());
  }
  QPen pen(widget->palette().color(QPalette::Text));
  pen.setWidth(0);
  painter->setPen(pen);
  QRectF keyFrameRect(this->boundingRect());
  painter->drawRect(keyFrameRect);

  QFontMetrics metrics(widget->font());
  double halfWidth = keyFrameRect.width() / 2.0 - 5;

  double iconWidth = keyFrameRect.width();

  QString label = metrics.elidedText(startValue().toString(), Qt::ElideRight, qRound(halfWidth));
  QPointF pt(keyFrameRect.left() + 3.0,
    keyFrameRect.top() + 0.5 * keyFrameRect.height() + metrics.height() / 2.0 - 1.0);
  painter->drawText(pt, label);

  iconWidth -= metrics.horizontalAdvance(label);

  label = metrics.elidedText(endValue().toString(), Qt::ElideRight, qRound(halfWidth));
  double hAdvance = metrics.horizontalAdvance(label);
  pt = QPointF(keyFrameRect.right() - hAdvance - 3.0,
    keyFrameRect.top() + 0.5 * keyFrameRect.height() + metrics.height() / 2.0 - 1.0);
  painter->drawText(pt, label);
  iconWidth -= hAdvance;

  if (iconWidth >= 16)
  {
    QPixmap pix = this->Icon.pixmap(16);
    painter->drawPixmap(keyFrameRect.center() - QPointF(8.0, 8.0), pix);
  }

  painter->restore();
}
