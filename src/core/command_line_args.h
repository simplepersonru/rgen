#pragma once

#include <llvm/Support/CommandLine.h>

namespace llvm::cl
{

static OptionCategory MyToolCategory("Опции для генерирования");

inline opt<std::string> Arg_dbDir("db_dir",
                                  desc("путь к директории, с которой начнется рекурсивный поиск файла compilation_database.json вверх по файловой системе, начиная с этой директории"),
                                  value_desc("путь к папке"),
                                  // FIXME Исправить после переезда тестов модуля GMock. Примечание: поскольку внутренние тесты не генерирую БД компиляции, то на этом моменте, они начнут падать.
                                  cat(MyToolCategory));

inline opt<std::string> Arg_modules("additional_modules",
                                        desc("Имена дополнительных модулей для обработки генератором. Модули по умолчанию используются всегда. Имена разделены запятыми"),
                                        cat(MyToolCategory));

inline opt<std::string> Arg_outputDir("output_dir",
                                      desc("указание директории, в которой окажется артефакт генерации"),
                                      value_desc("Путь к папке"),
                                      cat(MyToolCategory));

inline opt<bool> Arg_onlyArtifacts("only_artifacts",
                                   desc("Подать на выход генератора только имена артефактов"),
                                   cat(MyToolCategory));

inline opt<std::string> Arg_getAllModulesInfo("get_all_modules_info",
                                              desc("Поулчить информацию по модулям в виде файла. Формат файла задается с помощью ключа module_info_type"),
                                              cat(MyToolCategory));

inline opt<std::string> Arg_modulesInfoType("module_info_type",
                                            desc("Принимает ключ: json и js. По умолчанию json. Работает вместе с get_all_modules_info"),
                                            cat(MyToolCategory),
                                            init("json"));

inline list<std::string> Arg_fileForGen("input",
                                        desc("Путь к файлу которому нужно сделать генерацию"),
                                        cat(MyToolCategory));

inline opt<bool> Arg_verbose("verbose",
                             desc("Показать всю выводимую информацию по генерации"),
                             cat(MyToolCategory));

inline opt<bool> Arg_DebugPrintArtifactsWithHeader("debug_print_artifacts_with_header",
                                                   desc("Помимо папки назначения, распечатать артефакт так же рядом с хедером, но не сообщать об этом"),
                                                   cat(MyToolCategory));

inline opt<std::string> Arg_buildDirCurrentLib("build_dir_current_lib",
                                               desc("Указать сборочную директорию текущей библиотеки"),
                                               cat(MyToolCategory));

inline list<std::string> Arg_buildDirDependsLib("build_dir_other_lib",
                                                desc("Указать сборочную директорию зависимой библиотеки"),
                                                cat(MyToolCategory));

inline opt<std::string> Arg_templateDir("template_dir",
                                         desc("Путь к директории с шаблонами"),
                                         value_desc("Путь к папке"),
                                         cat(MyToolCategory));

inline opt<std::string> Arg_custom_commands("custom-commands",
                                      desc("JSON строка с MetaInfo от пользователя"),
                                      value_desc("JSON строка"),
                                      cat(MyToolCategory));


} // namespace llvm::cl

namespace cLine = llvm::cl;
