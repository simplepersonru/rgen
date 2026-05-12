#pragma once

#include <string>
#include <rgen/common/link_error_helper.h>
// Хедер <rgen/private/json_pre.h> отсуствует намеренно.

/*!
 * \brief Redkit-gen модуль Json
 * \warning Не для вызова, а для поддержки переходов по символу в Doxygen
 */
void Json();

namespace rgen
{

enum JsonFormat
{
    Compact,
    Pretty
};

template <typename S, class LinkErrorHelper = RedkitGenLinkErrorHelper>
class Json
{
public:
    static std::string toJson(const S& settings, JsonFormat format = JsonFormat::Compact)
    {
        std::string str;
        json j;
        fromTypeToJson(j, settings);

        if (format == JsonFormat::Pretty)
        {
            str = j.dump(4);
        }
        else // JsonFormat::Compact by default
        {
            str = j.dump();
        }

        return str;
    }

    static S fromJson(const std::string& str)
    {
        json j = json::parse(str);
        S out;
        fromJsonToType(j, out);
        return out;
    }

    /*!
     * \brief В зависимости от типа объекта - получить из него json
     * \param object - генерируемая структура или std::string
     */
    static nlohmann::json getJsonFromObject(S object)
    {
        if constexpr (std::is_same<S, std::string>::value)
            return nlohmann::json::parse(object);
        else
            return nlohmann::json::parse(toJson(object));
    }

    /*!
     * \brief Получить json diff из двух одинаковых структур
     */
    static nlohmann::json getPatch(S expected, S actual)
    {
        auto jExpected = getJsonFromObject(expected);
        auto jActual = getJsonFromObject(actual);

        return nlohmann::json::diff(jExpected, jActual);
    }

    /*!
     * \brief Функция для тестов! Производит сравнивание двух одинаковых структур и выводит несколько различий, если таковые имеются
     * \param stream - Поток ошибок, куда выводить сообщения
     * \param actual - Реальное содержимое структуры
     * \param expected - Ожидаемое содержимое структуры
     * \param diffCount - Количество записей различия для вывода
     * \param indent (default = 3) - Отступ (в количестве пробелов) при выводе различий
     * \return true - две структуры идентичны.
     * \return false - есть различия.
     * \code
     *      // Пример использования
     *      QVERIFY2(rgen::assertJsonEquals(expectedSettings, actualSettings), "Структуры не равны");
     * \endcode
     */
    template<typename ErrorStream>
    static bool assertJsonEquals(ErrorStream&& stream, S actual, S expected, size_t diffCount = 3, int indent = 3)
    {
        auto jPatch = getPatch(expected, actual);

        if (jPatch.size() == 0)
            return true;

        // Показать различия
        for (size_t i = 0; i < jPatch.size(); ++i)
        {
            // Вывести только diffCount записей различия
            if (i > diffCount)
                break;

            stream << jPatch[i].dump(indent).c_str();
        }
        return false;
    }
};

} // namespace rgen

/// For DERIVE support
#define RGEN_Json(prefix, name)

