#include <QApplication>
#include <QTimer>
#include <QTemporaryDir>
#include <sstream>

static void chooseWindow();

class CTestApplication : public QApplication {
  public:
    CTestApplication(int& argc, char** argv) : QApplication(argc, argv) {
        QTimer::singleShot(0, chooseWindow);
    }
};

// Exercise the real picker producer, including the checkbox and button ID map.
#define QApplication CTestApplication
#define main         pickerMain
#include "../abyssland-share-picker/main.cpp"
#undef main
#undef QApplication

static void chooseWindow() {
    for (const auto BUTTON : mainPickerPtr->findChildren<QPushButton*>()) {
        if (BUTTON->text() != "TestApp: Window with spaces")
            continue;
        BUTTON->click();
        return;
    }
    QApplication::exit(1);
}

int main() {
    QTemporaryDir runtime;
    if (!runtime.isValid())
        return 1;
    qputenv("QT_QPA_PLATFORM", "offscreen");
    qputenv("QT_QPA_PLATFORMTHEME", "");
    qputenv("QT_STYLE_OVERRIDE", "Fusion");
    qputenv("XDG_RUNTIME_DIR", runtime.path().toUtf8());
    qputenv("XDPH_WINDOW_SHARING_LIST", "4294967295[HC>]TestApp[HT>]Window with spaces[HE>]0[HA>]");

    char               program[] = "picker_gui_test", flag[] = "--allow-token";
    char*              argv[] = {program, flag, nullptr};
    std::ostringstream output;
    const auto         ORIGINAL = std::cout.rdbuf(output.rdbuf());
    const auto         RESULT   = pickerMain(2, argv);
    std::cout.rdbuf(ORIGINAL);
    if (RESULT || output.str() != "[SELECTION]r/window:4294967295\n")
        return 1;
    const auto SELECTION = PickerData::parseSelection(output.str());
    return SELECTION && SELECTION->allowToken && SELECTION->id == 4294967295U ? 0 : 1;
}
