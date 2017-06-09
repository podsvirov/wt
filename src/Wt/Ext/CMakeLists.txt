ADD_LIBRARY(wtext
  AbstractButton.C
  AbstractToggleButton.C
  AccordionLayoutImpl.C
  BorderLayoutImpl.C
  Button.C
  Calendar.C
  CheckBox.C
  ComboBox.C
  Component.C
  Container.C
  DataStore.C
  DateField.C
  DefaultLayoutImpl.C
  Dialog.C
  FitLayoutImpl.C
  FormField.C
  LayoutImpl.C
  LayoutItemImpl.C
  LineEdit.C
  Menu.C
  MenuItem.C
  MessageBox.C
  NumberField.C
  PagingToolBar.C
  Panel.C
  ProgressDialog.C
  RadioButton.C
  Splitter.C
  SplitterHandle.C
  TableView.C
  TabWidget.C
  TextEdit.C
  ToolBar.C
  ToolTipConfig.C
  Widget.C
  WWidgetItemImpl.C
)

TARGET_LINK_LIBRARIES(wtext wt ${BOOST_WT_LIBRARIES})

IF(NOT CMAKE_VERSION VERSION_LESS 2.8.12)
  SET_TARGET_PROPERTIES(wtext PROPERTIES EXPORT_NAME Ext)
  SET(_EXPORT_wt_target_ext EXPORT wt-target-ext)
ELSE()
  SET(_EXPORT_wt_target_ext)
ENDIF()

INSTALL(TARGETS wtext
    ${_EXPORT_wt_target_ext}
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION ${LIB_INSTALL_DIR}
    ARCHIVE DESTINATION ${LIB_INSTALL_DIR})

IF(NOT CMAKE_VERSION VERSION_LESS 2.8.12)
  INSTALL(EXPORT wt-target-ext
    DESTINATION ${CMAKE_INSTALL_DIR}/wt
    NAMESPACE Wt::)
ENDIF()
UNSET(_EXPORT_wt_target_ext)

IF(MSVC)
  SET_TARGET_PROPERTIES(wtext PROPERTIES COMPILE_FLAGS "${BUILD_PARALLEL} /wd4251 /wd4275 /wd4355 /wd4800 /wd4996 /wd4101 /wd4267")
ENDIF(MSVC)

SET_TARGET_PROPERTIES(
  wtext
PROPERTIES
  VERSION ${VERSION_SERIES}.${VERSION_MAJOR}.${VERSION_MINOR}
  SOVERSION ${WTEXT_SOVERSION}
  DEBUG_POSTFIX ${DEBUG_LIB_POSTFIX}
)

INSTALL_FILES(/include/Wt/Ext "^[^.]..+[^Ch~]$")
INSTALL_FILES(/include/Wt/Ext ExtDllDefs.h)
