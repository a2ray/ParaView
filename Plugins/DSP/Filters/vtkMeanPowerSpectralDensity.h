// SPDX-FileCopyrightText: Copyright (c) Kitware Inc.
// SPDX-License-Identifier: BSD-3-Clause

/**
 * @class   vtkMeanPowerSpectralDensity
 *
 * This filter computes the mean power spectral density (PSD) of temporal signals.
 * The input should be a multiblock dataset of vtkTables where each block
 * represents a point. Each row of the tables corresponds to a timestep.
 */

#ifndef vtkMeanPowerSpectralDensity_h
#define vtkMeanPowerSpectralDensity_h

#include "vtkDSPFiltersPluginModule.h"
#include "vtkTableAlgorithm.h"

class VTKDSPFILTERSPLUGIN_EXPORT vtkMeanPowerSpectralDensity : public vtkTableAlgorithm
{
public:
  static vtkMeanPowerSpectralDensity* New();
  vtkTypeMacro(vtkMeanPowerSpectralDensity, vtkTableAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  ///@{
  /**
   * Get/set the name of the FFT array from which to compute the mean PSD.
   * Default is empty.
   */
  vtkGetMacro(FFTArrayName, std::string);
  vtkSetMacro(FFTArrayName, std::string);
  ///@}

  ///@{
  /**
   * Get/set the name of the frequency array to copy to the output.
   * Default is empty.
   */
  vtkGetMacro(FrequencyArrayName, std::string);
  vtkSetMacro(FrequencyArrayName, std::string);
  ///@}

protected:
  vtkMeanPowerSpectralDensity() = default;
  ~vtkMeanPowerSpectralDensity() override = default;

  int FillInputPortInformation(int, vtkInformation*) override;
  int RequestData(vtkInformation*, vtkInformationVector**, vtkInformationVector*) override;

private:
  vtkMeanPowerSpectralDensity(const vtkMeanPowerSpectralDensity&) = delete;
  void operator=(const vtkMeanPowerSpectralDensity&) = delete;

  std::string FFTArrayName;
  std::string FrequencyArrayName;
};

#endif // vtkMeanPowerSpectralDensity_h
