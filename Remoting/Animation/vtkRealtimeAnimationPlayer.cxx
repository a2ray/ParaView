// SPDX-FileCopyrightText: Copyright (c) Kitware Inc.
// SPDX-License-Identifier: BSD-3-Clause
#include "vtkRealtimeAnimationPlayer.h"

#include "vtkObjectFactory.h"
#include "vtkTimerLog.h"

namespace
{
const double endTimeThresholdFactor = 1.1;
const double startTimeThresholdOffset = 0.1;
}

vtkStandardNewMacro(vtkRealtimeAnimationPlayer);
//----------------------------------------------------------------------------
vtkRealtimeAnimationPlayer::vtkRealtimeAnimationPlayer()
{
  this->StartTime = 0;
  this->EndTime = 1.0;
  this->ShiftTime = 0.0;
  this->Factor = 1.0;
  this->Duration = 1;
  this->Timer = vtkTimerLog::New();
}

//----------------------------------------------------------------------------
vtkRealtimeAnimationPlayer::~vtkRealtimeAnimationPlayer()
{
  this->Timer->Delete();
}

//----------------------------------------------------------------------------
void vtkRealtimeAnimationPlayer::StartLoop(
  double start, double end, double curtime, double* playbackWindow)
{
  this->StartTime = start;
  this->Factor = (end - start) / this->Duration;

  // set a time shift to resume an interrupted animation (fix to bug #0008280)
  if (start < curtime && curtime <= end)
  {
    this->ShiftTime = curtime - this->StartTime;
  }
  else
  {
    this->ShiftTime = 0.0;
  }

  // obtain the end time to be used in GetNextTime(...)
  this->EndTime = playbackWindow[1];

  this->Timer->StartTimer();
}

//----------------------------------------------------------------------------
double vtkRealtimeAnimationPlayer::GetNextTime(double curtime)
{
  // The following line, in support of resuming an interrupted animation, forces
  // the animation to terminate by just breaking the while-loop,
  // while (!this->StopPlay && this->CurrentTime <= endtime) in
  // vtkAnimationPlayer::Play(), WITHOUT affecting the actual scene / tick time.
  // This line MUST !!NOT!! be removed, otherwise a crash problem would occur.
  if (curtime == this->EndTime)
  {
    return this->EndTime * endTimeThresholdFactor;
  }

  this->Timer->StopTimer();
  double elapsed = this->Timer->GetElapsedTime();

  // in support of resuming an interrupted animation
  double nextTime = this->StartTime + this->ShiftTime + this->Factor * elapsed;

  // The if-statement below, in support of resuming an interrupted animation,
  // forces the LAST animation step to reach exactly 'this->EndTime', which enables
  // the while-loop, 'while (!this->StopPlay && this->CurrentTime <= endtime)' in
  // vtkAnimationPlayer::Play(). The execution of this very LAST cycle of the
  // while-loop body ensures the animation tick to reach exactly 'this->EndTime'
  // and therefore ensures later access to the correct scene time
  // ('this->EndTime') via this->CurrentTime = this->AnimationScene->GetSceneTime()
  // in vtkAnimationPlayer::Play(). This if-statement MUST !!NOT!! be removed.
  // Otherwise the animation, sometimes, could not be re-started from the very
  // beginning as 'this->ShiftTime' would not be inited to zero.
  return (nextTime > this->EndTime) ? this->EndTime : nextTime;
}

//----------------------------------------------------------------------------
double vtkRealtimeAnimationPlayer::GetPreviousTime(double curtime)
{
  // The following line, in support of resuming an interrupted animation, forces
  // the animation to terminate by just breaking the while-loop,
  // while (!this->StopPlay && this->CurrentTime >= startTime) in
  // vtkAnimationPlayer::Play(), WITHOUT affecting the actual scene / tick time.
  // This line MUST !!NOT!! be removed, otherwise a crash problem would occur.
  if (curtime == this->StartTime)
  {
    // starttime can be 0. So, offset is subtracted from
    // starttime and not multiplied as done in `GetNextTime`.
    return this->StartTime - startTimeThresholdOffset;
  }

  this->Timer->StopTimer();
  double elapsed = this->Timer->GetElapsedTime();

  // in support of resuming an interrupted animation
  double prevTime = this->StartTime + this->ShiftTime - this->Factor * elapsed;

  // The if-statement below, in support of resuming an interrupted animation,
  // forces the LAST animation step to reach exactly 'this->StartTime', which enables
  // the while-loop, 'while (!this->StopPlay && this->CurrentTime <= startTime)' in
  // vtkAnimationPlayer::Play(). The execution of this very LAST cycle of the
  // while-loop body ensures the animation tick to reach exactly 'this->StartTime'
  // and therefore ensures later access to the correct scene time
  // ('this->StartTime') via this->CurrentTime = this->AnimationScene->GetSceneTime()
  // in vtkAnimationPlayer::Play(). This if-statement MUST !!NOT!! be removed.
  // Otherwise the animation, sometimes, could not be re-started from the very
  // beginning as 'this->ShiftTime' would not be inited to zero.
  return (prevTime < this->StartTime) ? this->StartTime : prevTime;
}

//----------------------------------------------------------------------------
double vtkRealtimeAnimationPlayer::GoToNext(double, double, double currenttime)
{
  return (currenttime + 1);
}

//----------------------------------------------------------------------------
double vtkRealtimeAnimationPlayer::GoToPrevious(double, double, double currenttime)
{
  return (currenttime - 1);
}

//----------------------------------------------------------------------------
void vtkRealtimeAnimationPlayer::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}
