TEMPLATE = subdirs

# Очередность сборки: сначала приложение, потом тесты
SUBDIRS = src tests

main_app.subdir = src
main_app.file = src/untitled.pro

tests.subdir = tests
tests.file = tests/tests.pro
tests.depends = main_app