#ifndef TEST_IO_HELPERS_H
#define TEST_IO_HELPERS_H

/* #include <IO/rf_file.h> */
#include <Utils/rf_unicode.h>
#include <Utils/endianess.h>

#define FIRST_LINE_UTF8 "UTF-8 File"

#define SECOND_LINE_UTF8 \
    "２日の東京外国為替市場では、主要通貨に対して"                      \
    "円が売られている。午前１０時現在、ドルに対する円"                  \
    "相場は前日午後５時時点に比べ２９銭円安ドル高の"                    \
    "１ドル＝８０円２６～２７銭。ユーロに対しては同２８銭円"            \
    "安ユーロ高の１ユーロ＝１０３円７６～８０銭。"

#define SECOND_LINE_UTF16 \
    "Κανένα περιθώριο για διαπραγμάτευση δεν αφήνει το ΔΝΤ"         \
    " το οποίο ξεκαθαρίζει πως οι αλλαγές στα εργασιακά αποτελούν " \
    "αναπόσπαστο κομμάτι της συμφωνίας Ελλάδας και τρόικας."        \

#define SECOND_LINE_UTF32_BE                                      \
    "また米兵が沖縄の人の暮らしを脅かした。２日未明、酔った米"    \
    "兵が民家に押し入り、中学生を殴ったとされる事件。半月前"      \
    "の米兵による集団強姦（ごうかん）事件を受けて在日米軍に出た"  \
    "外出禁止令を破る振る舞いに、「米兵には綱紀も何もない」"      \
    "と憤りの声が上がった。"

#define SECOND_LINE_UTF32_LE                                              \
    "Η χώρα κρέμεται πάλι από μια κλωστή. Η σημερινή ψηφοφορία"         \
    " στην Βουλή δείχνει πως το πολιτικό σύστημα επιμένει στην αυτοκτονία," \
    " τη δική του και της χώρας."


#define THIRD_LINE_UTF8                                                 \
    "Αντισυνταγματικές κρίθηκαν σχεδόν ομόφωνα από την Ολομέλεια "      \
    "του Ελεγκτικού Συνεδρίου, σύμφωνα με πληροφορίες, οι διατάξεις "   \
    "του υπουργείου Οικονομικών με τις οποίες επέρχονται νέες μειώσεις " \
    "στις συντάξεις, της τάξεως του 5% έως 10%, ενώ με την είσοδο του " \
    "νέου έτους αυξάνεται το όριο συνταξιοδότησης κατά δύο έτη στα 67 " \
    "και καταργούνται για τους συνταξιούχους τα επιδόματα (δώρα) "      \
    "Χριστουγέννων και Πάσχα, όπως και το επίδομα θερινής άδειας."

#define THIRD_LINE_UTF16                            \
    "米国の経済指標が市場予想を上回ったことから、"  \
    "米景気が緩やかに改善するとの期待が広がり、"    \
    "ドルが買われ円が売られた。"

#define THIRD_LINE_UTF32_LE                                             \
    "中塚一宏金融相は２日の閣議後の記者会見で、インターネットバンキング" \
    "で偽画面を使った不正送金が相次いでいることを受け、すべての銀行に対し" \
    "、同じような被害があるかどうかを報告するよう求めたことを明らかにした。"

#define THIRD_LINE_UTF32_BE                                             \
    "Για την επίπτωση της διαπραγμάτευσης κυβέρνησης - τρόικας για τα " \
    "εργασιακά στον κυβερνητικό συνασπισμό είπε πως αυτό αφορά την "   \
    "ίδια την κυβέρνηση."

static const char *line_scenario1[] = {
    "UTF-8 File\n",
    "Little Endian UTF-16 file\n",
    "Big Endian UTF-16 file\n",
    /* It's actually opposite, file has wrong string inside  */
    "Little Endian UTF-32 string file\n",
    /* It's actually opposite, file has wrong string inside  */
    "Big Endian UTF-32 string file\n",


    "UTF-8 File",
    "Little Endian UTF-16 file",
    "Big Endian UTF-16 file",
    /* It's actually opposite, file has wrong string inside  */
    "Little Endian UTF-32 string file",
    /* It's actually opposite, file has wrong string inside  */
    "Big Endian UTF-32 string file"
};

static const char *line_scenario2[] = {
    SECOND_LINE_UTF8"\n",
    SECOND_LINE_UTF16"\n",
    SECOND_LINE_UTF16"\n",
    /* It's actually opposite, file has wrong string inside  */
    SECOND_LINE_UTF32_BE"\n",
    /* It's actually opposite, file has wrong string inside  */
    SECOND_LINE_UTF32_LE"\n",


    SECOND_LINE_UTF8,
    SECOND_LINE_UTF16,
    SECOND_LINE_UTF16,
    /* It's actually opposite, file has wrong string inside  */
    SECOND_LINE_UTF32_BE,
    /* It's actually opposite, file has wrong string inside  */
    SECOND_LINE_UTF32_LE,
};

static const char *line_scenario3[] = {
    THIRD_LINE_UTF8"\n",
    THIRD_LINE_UTF16"\n",
    THIRD_LINE_UTF16"\n",
    /* It's actually opposite, file has wrong string inside  */
    THIRD_LINE_UTF32_BE"\n",
    /* It's actually opposite, file has wrong string inside  */
    THIRD_LINE_UTF32_LE"\n",


    THIRD_LINE_UTF8,
    THIRD_LINE_UTF16,
    THIRD_LINE_UTF16,
    /* It's actually opposite, file has wrong string inside  */
    THIRD_LINE_UTF32_BE,
    /* It's actually opposite, file has wrong string inside  */
    THIRD_LINE_UTF32_LE,
};

inline static const char *get_line(int encoding,
                                   int endianess,
                                   bool with_newline,
                                   const char* scenario[])
{
    if (encoding == RF_UTF8) {
        return with_newline ? scenario[0] : scenario[5];
    } else if (encoding == RF_UTF16 && endianess == RF_LITTLE_ENDIAN) {
        return with_newline ? scenario[1] : scenario[6];
    } else if (encoding == RF_UTF16 && endianess == RF_BIG_ENDIAN) {
        return with_newline ? scenario[2] : scenario[7];
    } else if (encoding == RF_UTF32 && endianess == RF_LITTLE_ENDIAN) {
        return with_newline ? scenario[4] : scenario[9];
    }

    return with_newline ? scenario[3] : scenario[8];
}

inline static uint32_t get_uint32_array(int encoding,
                                        int endianess,
                                        const uint32_t* arr)
{
    if (encoding == RF_UTF8) {
        return arr[0];
    } else if (encoding == RF_UTF16 && endianess == RF_LITTLE_ENDIAN) {
        return arr[1];
    } else if (encoding == RF_UTF16 && endianess == RF_BIG_ENDIAN) {
        return arr[2];
    } else if (encoding == RF_UTF32 && endianess == RF_LITTLE_ENDIAN) {
        return arr[4];
    }
    return arr[3];
}

#endif
