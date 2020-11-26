#include "args.h"

#include "ext/CLI11.hpp"

namespace Args {
  std::string rulesFilePath = "/etc/amidiminder.rules";
  bool rulesCheckOnly = false;

  int exitCode = 0;

  bool parse(int argc, char* argv[]) {
    CLI::App app{"bicycle - a MIDI looper"};

    app.add_option("-f,--rules-file", rulesFilePath, "defaults to " + rulesFilePath);
    app.add_flag("-C,--check-rules", rulesCheckOnly, "check rules file then edit");

    try {
        app.parse(argc, argv);
    } catch (const CLI::ParseError &e) {
        exitCode = app.exit(e);
        return false;
    }

    return true;
  }
}
