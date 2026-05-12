#pragma once

#include <rgen/common/link_error_helper.h>

#include <optional>
#include <string>
#include <vector>

/*!
 * \warning Не для вызова, а для поддержки переходов по символу в Doxygen,
 * см документацию ниже
 */
void Enum();

namespace rgen
{

/*!
 * \brief Сериализатор enum
 */
template <typename EnumType, class LinkErrorHelper = RedkitGenLinkErrorHelper>
class Enum
{
public:
    /*!
     * \brief Получить элемент enum из строки
     * \param value - строковое значение
     * \param defaultResult - вернется по умолчанию, если не нашлось соответствия
     * \throw std::runtime_error если defaultResult не задан и не нашлось соответствия
     */
    static EnumType fromString(std::string value, std::optional<EnumType> defaultResult = {});

    /*!
     * \brief Переводит элемент enum в строку
     * \param value - элемент enum
     * \return строку
     *
     * Если для элемента enum есть обычный комментарий,
     * либо doxygen комментарий с аттрибутом @Display,
     * то именно это значение будет возвращено
     */
    static std::string toString(EnumType value);

    /*!
     * \brief Возвращает вектор всех значений enum
     */
    static std::vector<EnumType> values();
};

} // namespace rgen
