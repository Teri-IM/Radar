TEMPLATE = subdirs

# Очередность сборки: сначала приложение, потом тесты
SUBDIRS = src tests

# Говорим qmake, что тесты зависят от сборки основного кода
tests.depends = src