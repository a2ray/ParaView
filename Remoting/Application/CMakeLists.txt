# SPDX-FileCopyrightText: Copyright (c) Kitware Inc.
# SPDX-FileCopyrightText: Copyright (c) Sandia Corporation
# SPDX-License-Identifier: BSD-3-Clause
set(classes
  vtkInitializationHelper)

set(private_headers
  vtkPVInitializer.h)

vtk_module_add_module(ParaView::RemotingApplication
  CLASSES ${classes}
  PRIVATE_HEADERS ${private_headers})
vtk_module_link(ParaView::RemotingApplication
  PRIVATE
    paraview_client_server
    paraview_server_manager)
vtk_module_client_server_exclude()

set(xml_files
  Resources/extractors.xml
  Resources/internal_writers.xml
  Resources/utilities.xml)

if (TARGET VTK::CommonComputationalGeometry)
  list(APPEND xml_files
    Resources/proxies_computationalgeometry.xml)
endif ()

if (TARGET VTK::DomainsChemistry)
  list(APPEND xml_files
    Resources/filters_domainschemistry.xml)
endif ()

if (TARGET VTK::IOChemistry)
  list(APPEND xml_files
    Resources/readers_iochemistry.xml)
endif ()

if (TARGET VTK::FiltersAMR)
  list(APPEND xml_files
    Resources/filters_filtersamr.xml
    Resources/sources_filtersamr.xml)
endif ()

if (TARGET VTK::FiltersCore)
  list(APPEND xml_files
    Resources/filters_filterscore.xml
    Resources/sources_filterscore.xml)
endif ()

if (TARGET VTK::FiltersPoints)
  list(APPEND xml_files
    Resources/filters_filterspoints.xml)
endif ()

if (TARGET VTK::FiltersExtraction)
  list(APPEND xml_files
    Resources/filters_filtersextraction.xml)
endif ()

if (TARGET VTK::FiltersFlowPaths)
  list(APPEND xml_files
    Resources/filters_filtersflowpaths.xml)
endif ()

if (TARGET VTK::FiltersGeneral)
  list(APPEND xml_files
    Resources/filters_filtersgeneral.xml
    Resources/sources_filtersgeneral.xml)
endif ()

if (TARGET VTK::FiltersGeneral AND
    TARGET VTK::FiltersGeometry AND
    TARGET ParaView::VTKExtensionsFiltersGeneral)
  list(APPEND xml_files
    Resources/filters_plotonintersectioncurves.xml)
endif ()

if (TARGET ParaView::VTKExtensionsFiltersGeneral AND
    TARGET VTK::FiltersGeneral AND
    TARGET VTK::FiltersParallel)
  list(APPEND xml_files
    Resources/filters_normalglyphs.xml)
endif()

if (TARGET VTK::FiltersGeneric)
  list(APPEND xml_files
    Resources/filters_filtersgeneric.xml)
endif ()

if (TARGET VTK::FiltersGeometry)
  list(APPEND xml_files
    Resources/filters_filtersgeometry.xml)
endif ()

if (TARGET VTK::FiltersParallel)
  list(APPEND xml_files
    Resources/filters_filtersparallel.xml)
endif ()

if (TARGET VTK::FiltersParallelMPI)
  list(APPEND xml_files
    Resources/filters_filtersparallelmpi.xml)
endif ()

if (TARGET VTK::FiltersHybrid)
  list(APPEND xml_files
    Resources/filters_filtershybrid.xml
    Resources/readers_filtershybrid.xml)
endif ()

if (TARGET VTK::FiltersHyperTree)
  list(APPEND xml_files
    Resources/filters_filtershypertree.xml)
endif ()

if (TARGET VTK::FiltersModeling)
  list(APPEND xml_files
    Resources/filters_filtersmodeling.xml)
endif ()

if (TARGET VTK::FiltersSources)
  list(APPEND xml_files
    Resources/sources_filterssources.xml)
endif ()

if (TARGET VTK::FiltersStatistics)
  list(APPEND xml_files
    Resources/filters_filtersstatistics.xml)
endif ()

if (TARGET VTK::ImagingCore)
  list(APPEND xml_files
    Resources/filters_imagingcore.xml
    Resources/sources_imagingcore.xml)
endif ()

if (TARGET VTK::ImagingFourier)
  list(APPEND xml_files
    Resources/filters_imagingfourier.xml)
endif ()

if (TARGET VTK::ImagingGeneral)
  list(APPEND xml_files
    Resources/filters_imaginggeneral.xml)
endif ()

if (TARGET VTK::ImagingHybrid)
  list(APPEND xml_files
    Resources/filters_imaginghybrid.xml)
endif ()

if (TARGET VTK::ImagingSources)
  list(APPEND xml_files
    Resources/sources_imagingsources.xml)
endif ()

if (TARGET VTK::ImagingStatistics)
  list(APPEND xml_files
    Resources/filters_imagingstatistics.xml)
endif ()

if (TARGET VTK::FiltersParallelDIY2)
  list(APPEND xml_files
    Resources/filters_filtersparalleldiy2.xml)
endif ()

if (TARGET VTK::FiltersParallelVerdict)
  list(APPEND xml_files
    Resources/filters_filtersparallelverdict.xml)
endif ()

if (TARGET VTK::FiltersTensor)
  list(APPEND xml_files
    Resources/filters_filterstensors.xml)
endif ()

if (TARGET VTK::FiltersTexture)
  list(APPEND xml_files
    Resources/filters_filterstexture.xml)
endif ()

if (TARGET VTK::FiltersVerdict)
  list(APPEND xml_files
    Resources/filters_filtersverdict.xml)
endif ()

if (TARGET VTK::IOXML AND TARGET VTK::IOParallelXML)
  list(APPEND xml_files
    Resources/readers_ioxml.xml
    Resources/writers_ioxml.xml)
endif ()

if (TARGET VTK::IONetCDF)
  list(APPEND xml_files
    Resources/readers_ionetcdf.xml)
endif ()

if (TARGET VTK::IOParallelNetCDF)
  list(APPEND xml_files
    Resources/readers_ioparallelnetcdf.xml)
endif ()

if (TARGET VTK::IOXdmf2)
  list(APPEND xml_files
    Resources/proxies_xdmf2.xml)
endif ()

if (TARGET VTK::IOXdmf3)
  list(APPEND xml_files
    Resources/proxies_xdmf3.xml)
endif ()

if (TARGET VTK::IOGDAL)
  list(APPEND xml_files
    Resources/proxies_gdal.xml)
endif ()

if (TARGET VTK::IOLAS)
  list(APPEND xml_files
    Resources/proxies_las.xml)
endif ()

if (TARGET VTK::IOPDAL)
  list(APPEND xml_files
    Resources/proxies_pdal.xml)
endif ()

if (TARGET VTK::IOCityGML)
  list(APPEND xml_files
    Resources/proxies_citygml.xml)
endif ()

if (TARGET VTK::IOHDF)
  list(APPEND xml_files
    Resources/readers_iohdf.xml)
endif ()

if (TARGET VTK::IOSegY)
  list(APPEND xml_files
    Resources/proxies_segy.xml)
endif ()

if (TARGET VTK::IOADIOS2)
  list(APPEND xml_files
    Resources/proxies_adios2_core.xml)
  if(PARAVIEW_USE_MPI)
    list(APPEND xml_files
      Resources/proxies_adios2_vtx.xml)
  endif()
endif ()

if (TARGET VTK::IOFides)
  list(APPEND xml_files
    Resources/proxies_fides.xml)
endif()

if (TARGET VTK::IOOMF)
  list(APPEND xml_files
    Resources/proxies_omf.xml)
endif()

if (TARGET VTK::IOMotionFX)
  list(APPEND xml_files
    Resources/proxies_motionfx.xml)
endif ()

if (TARGET VTK::IOLegacy)
  list(APPEND xml_files
    Resources/writers_iolegacy.xml)
endif()

if (TARGET VTK::IOGeometry)
  list(APPEND xml_files
    Resources/readers_iogeometry.xml
    Resources/writers_iogeometry.xml)
endif()

if (TARGET VTK::IOOCCT)
  list(APPEND xml_files
    Resources/readers_ioocct.xml)
endif ()

if (TARGET VTK::IOPLY)
  list(APPEND xml_files
    Resources/readers_ioply.xml)
endif()

if (TARGET VTK::IOParallel)
  list(APPEND xml_files
    Resources/readers_ioparallel.xml
    Resources/writers_ensight.xml)
endif()

if (TARGET VTK::IOParallelLSDyna)
  list(APPEND xml_files
    Resources/readers_lsdyna.xml)
endif()

if (TARGET VTK::IOInfovis)
  list(APPEND xml_files
    Resources/readers_ioinfovis.xml)
endif ()

if (TARGET VTK::IOTecplotTable)
  list(APPEND xml_files
    Resources/readers_iotecplottable.xml)
endif()

if (TARGET VTK::IOVPIC)
  list(APPEND xml_files
    Resources/readers_iovpic.xml)
endif()

if (TARGET VTK::IOTRUCHAS)
  list(APPEND xml_files
    Resources/readers_iotruchas.xml)
endif()

if (TARGET VTK::IOVeraOut)
  list(APPEND xml_files
    Resources/readers_ioveraout.xml)
endif()

if (TARGET VTK::IOH5part)
  list(APPEND xml_files
    Resources/readers_ioh5part.xml)
endif()

if (TARGET VTK::IOH5Rage)
  list(APPEND xml_files
    Resources/readers_ioh5rage.xml)
endif()

if (TARGET VTK::IOPIO)
  list(APPEND xml_files
    Resources/readers_iopio.xml)
endif()

if (TARGET VTK::IOCONVERGECFD)
  list(APPEND xml_files
    Resources/readers_ioconvergecfd.xml)
endif()

if (TARGET VTK::IOCGNSReader)
  list(APPEND xml_files
    Resources/readers_iocgns.xml)
endif()

if (TARGET VTK::IOFLUENTCFF)
  list(APPEND xml_files
    Resources/readers_iofluentcff.xml)
endif()

if (TARGET ParaView::VTKExtensionsIOCGNSWriter)
  list(APPEND xml_files
    Resources/writers_iocgns.xml)
endif()

if (TARGET VTK::FiltersOpenTURNS)
  list(APPEND xml_files
    Resources/proxies_openturns.xml)
endif ()

if (TARGET VTK::MomentInvariants)
  list(APPEND xml_files
    Resources/MomentInvariants.xml)
endif ()

if (TARGET VTK::ParallelMomentInvariants)
  list(APPEND xml_files
    Resources/ParallelMomentInvariants.xml)
endif ()

if (TARGET VTK::PythonInterpreter)
  _vtk_module_set_module_property(ParaView::RemotingApplication APPEND
    PROPERTY  "forward_link"
    VALUE     VTK::PythonUsed)
endif ()

if (TARGET VTK::RenderingFreeType)
  list(APPEND xml_files
    Resources/sources_renderingfreetype.xml)
endif ()

if (TARGET VTK::InteractionWidgets)
  list(APPEND xml_files
    Resources/proxies_interactionwidgets.xml)
endif()

if (TARGET VTK::FiltersParallel AND
    TARGET VTK::ImagingFourier AND
    TARGET ParaView::VTKExtensionsExtraction)
  list(APPEND xml_files
    Resources/filters_selectionfftovertime.xml)
endif()

if (TARGET VTK::IOIOSS)
  list(APPEND xml_files
    Resources/readers_ioioss.xml
    Resources/writers_ioioss.xml)
endif()

if (TARGET VTK::IOOpenVDB)
  list(APPEND xml_files
    Resources/readers_ioopenvdb.xml
    Resources/writers_ioopenvdb.xml)
endif()

if (TARGET VTK::IOCatalystConduit)
  list(APPEND xml_files
    Resources/proxies_conduit.xml)
endif()

if (TARGET VTK::IOFDS)
  list(APPEND xml_files
    Resources/readers_iofds.xml)
endif()

if (PARAVIEW_USE_PYTHON)
  list(APPEND xml_files
    Resources/readers_openpmd.xml
    Resources/readers_savg.xml)
  list(APPEND xml_files
    Resources/extractors_python.xml)
endif()

paraview_add_server_manager_xmls(
  XMLS  ${xml_files})
