restore_match_test = executable('restore_match_test',
  'restore_match_test.cpp', '../src/shared/RestoreMatch.cpp')
input_capture_geometry_test = executable('input_capture_geometry_test',
  'input_capture_geometry_test.cpp', '../src/shared/InputCaptureGeometry.cpp')
picker_data_test = executable('picker_data_test',
  'picker_data_test.cpp', '../src/shared/PickerData.cpp')
restore_data_test = executable('restore_data_test',
  'restore_data_test.cpp', '../src/shared/RestoreData.cpp',
  dependencies: dependency('sdbus-c++'))
desktop_identity_test = executable('desktop_identity_test',
  'desktop_identity_test.cpp', '../src/helpers/MiscFunctions.cpp',
  dependencies: [dependency('abyssutils'), dependency('sdbus-c++'), dependency('uuid')])
picker_gui_test = executable('picker_gui_test',
  'picker_gui_test.cpp', '../src/shared/PickerData.cpp',
  '../abyssland-share-picker/mainpicker.cpp',
  '../abyssland-share-picker/elidedbutton.cpp',
  ui_files, moc,
  include_directories: include_directories('../abyssland-share-picker'),
  dependencies: [qtdep, dependency('abyssutils')])

test('restore_match', restore_match_test)
test('input_capture_geometry', input_capture_geometry_test)
test('picker_data', picker_data_test)
test('restore_data', restore_data_test)
test('desktop_identity', desktop_identity_test)
test('picker_gui', picker_gui_test)
