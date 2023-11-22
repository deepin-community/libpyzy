/* vim:set et ts=4 sts=4:
 *
 * libpyzy - The Chinese PinYin and Bopomofo conversion library.
 *
 * Copyright (c) 2008-2010 Peng Huang <shawn.p.huang@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301
 * USA
 */
/**
 * \brief Converts Pinyin / Bopomofo characters to phrases.
 *
 * Takes PinYin / Bopomofo characters and convert it to phrases.
 * This class provides following methods.
 * - Context manipulation methods like insert(), removeCharBefore(), ...
 * - Getters of input / preedit / candidates / cursor / ...
 * - Getters / Setters of some conversion options.
 *
 * To use this class, you should call InputContext::init() at first and
 * InputContext::finalize() at last.
 */
#ifndef __PYZY_INPUT_CONTEXT_H_
#define __PYZY_INPUT_CONTEXT_H_

#include <string>
#include <vector>

namespace PyZy {

class Variant;

/**
 * \brief An enum to represent a candidate type.
 */
enum CandidateType {
    /** Candidate from a system dictionary. */
    NORMAL_PHRASE,
    /** Candidate from a user input. */
    USER_PHRASE,
    /** Candidate from a special phrase. */
    SPECIAL_PHRASE,
};

/**
 * \brief Contains a candidate data.
 */
struct Candidate {
    /** Text of a candidate */
    std::string text;
    /** Type of a candidate */
    CandidateType type;
};

/**
 * \brief Pinyin / Bopomofo conversion class.
 *
 * This class is a factory class. You call a create() method and you get a
 * pointer of the instance. You should delete it yourself.
 * If some context are modified, you can get a nortification through Observer
 * instance.
 */
class InputContext {
public:
    /**
     * \brief Virtual destructor.
     */
    virtual ~InputContext (void) { }

    /**
     * \brief Observer class of the InputContext.
     *
     * You override this class and pass it to InputContext::create, and you
     * can get a nortification as a callback through this instance when some
     * context are modified.
     */
    class Observer {
    public:
        virtual ~Observer () { }


        /**
         * \brief Notifies a commit text.
         * @param context InputContext instance which triggered this method.
         * @param commit_text Commited text.
         *
         * This method is triggered by InputContext when conversion result
         * is commited.
         */
        virtual void commitText (InputContext * context,
                                 const std::string &commit_text) = 0;

        /**
         * \brief Notifies input text is changed.
         * @param context InputContext instance which triggered this method.
         *
         * This method is triggered by InputContext when input text is
         * changed.
         */
        virtual void inputTextChanged (InputContext * context) = 0;

        /**
         * \brief Notifies cursor is changed.
         * @param context InputContext instance which triggered this method.
         *
         * This method is triggered by InputContext when cursor is changed.
         */
        virtual void cursorChanged (InputContext * context) = 0;

        /**
         * \brief Notifies preedit text is changed.
         * @param context InputContext instance which triggered this method.
         *
         * This method is triggered by InputContext when preedit text is
         * changed.
         */
        virtual void preeditTextChanged (InputContext * context) = 0;

        /**
         * \brief Notifies auxiliary text is changed.
         * @param context InputContext instance which triggered this method.
         *
         * This method is triggered by InputContext when auxiliary text is
         * changed.
         */
        virtual void auxiliaryTextChanged (InputContext * context) = 0;

        /**
         * \brief Notifies candidates are changed.
         * @param context InputContext instance which triggered this method.
         *
         * This method is triggered by InputContext when candidates are
         * changed.
         */
        virtual void candidatesChanged (InputContext * context) = 0;
    };

    /**
     * \brief Input text type.
     */
    enum InputType {
        /** Input text is full pinyin. */
        FULL_PINYIN,
        /** Input text is double pinyin. */
        DOUBLE_PINYIN,
        /** Input text is bopomofo. */
        BOPOMOFO,
    };

    /**
     * \brief Commit type.
     */
    enum CommitType {
        /** Commits a input text directly. */
        TYPE_RAW,
        /** Commits a phonetic symbols, mainly used for Bopomofo. */
        TYPE_PHONETIC,
        /** Commits a selected text, focused conversion text and rest text. */
        TYPE_CONVERTED,
    };

    /**
     * \brief PropertyName
     */
    enum PropertyName {
        /**
         * Conversion option
         * @see Const.h
         *
         * Default value is PINYIN_INCORRECT_PINYIN | PINYIN_CORRECT_ALL |
         * PYZY_FUZZY_ALL
         */
        PROPERTY_CONVERSION_OPTION,
        /**
         * \brief Double pinyin schema
         * @see Const.h
         *
         * Default value is DOUBLE_PINYIN_KEYBOARD_MSPY.
         */
        PROPERTY_DOUBLE_PINYIN_SCHEMA,
        /**
         * \brief Bopomofo schema
         * @see Const.h
         *
         * Default value is BOPOMOFO_KEYBOARD_STANDARD.
         */
        PROPERTY_BOPOMOFO_SCHEMA,
        /**
         * \brief Uses special phrase.
         * Default value is true.
        */
        PROPERTY_SPECIAL_PHRASE,
        /**
         * \brief Uses simplified chinese character.
         * Default value is true.
         */
        PROPERTY_MODE_SIMP,
    };

    /**
     * \brief Appends a new character on cursor position.
     * @param ch Input character. It should be ASCII characters.
     * @return true if succeed.
     *
     * Appends a new character. This method fails if there are too many
     * characters or invalid character is input.
     */
    virtual bool insert (char ch) = 0;

    /**
     * \brief Fixes the conversion result.
     * @param type Commit type.
     * @see CommitType
     *
     * Fixes the conversion result according to CommitType and resets the
     * context.
     */
    virtual void commit (CommitType type = TYPE_CONVERTED) = 0;

    /**
     * \brief Resets the context.
     */
    virtual void reset (void) = 0;

    /**
     * \brief Moves a cursor to right input character.
     * @return true if cursor is moved.
     */
    virtual bool moveCursorRight (void) = 0;

    /**
     * \brief Moves a cursor to left input character.
     * @return true if cursor is moved.
     */
    virtual bool moveCursorLeft (void) = 0;

    /**
     * \brief Moves a cursor to left chinese character.
     * @return true if cursor is moved.
     * @see moveCursorToEnd
     *
     * Currently text after a cursor is not converted and it is assumed as one
     * chinese character. So this method is same as moveCursorToEnd.
     */
    virtual bool moveCursorRightByWord (void) = 0;

    /**
     * \brief Moves a cursor to right chinese character.
     * @return true if cursor is moved.
     */
    virtual bool moveCursorLeftByWord (void) = 0;

    /**
     * \brief Moves a cursor to the beginning of the the input text.
     * @return true if cursor is moved.
     */
    virtual bool moveCursorToBegin (void) = 0;

    /**
     * \brief Removes a input character before a cursor.
     * @return true if a character is removed.
     */
    virtual bool removeCharBefore (void) = 0;

    /**
     * \brief Removes a input character after a cursor.
     * @return true if a character is removed.
     */
    virtual bool removeCharAfter (void) = 0;

    /**
     * \brief Removes a chinese character before a cursor.
     * @return true if a character is removed.
     */
    virtual bool removeWordBefore (void) = 0;

    /**
     * \brief Removes a chinese character after a cursor.
     * @return true if a character is removed.
     *
     * Currently text after a cursor is not converted and it is assumed as one
     * chinese character. So this method removes all text after a cursor.
     */
    virtual bool removeWordAfter (void) = 0;

    /**
     * \brief Moves a cursor to the end of the the input text.
     * @return true if cursor is moved.
     */
    virtual bool moveCursorToEnd (void) = 0;

    /**
     * \brief Selects a candidate and converts a rest text.
     * @param index Index of the candidate. (0-origin)
     * @return true if it takes valid index.
     * @see commit
     *
     * If there are no rest text after select, this method triggeres commit.
     */
    virtual bool selectCandidate (size_t index) = 0;

    /**
     * \brief Focuses a candidate.
     * @param index Index of the candidate. (0-origin)
     * @return true if it takes valid index.
     */
    virtual bool focusCandidate (size_t index) = 0;

    /**
     * \brief Focuses a previous candidate.
     * @return true if focused index is changed.
     */
    virtual bool focusCandidatePrevious (void) = 0;

    /**
     * \brief Focuses a next candidate.
     * @return true if there are some candidates after the focused candidate;
     *           false otherwise.
     */
    virtual bool focusCandidateNext (void) = 0;

    /**
     * \brief Resets a user input history of the candidate.
     * @param index Index of the candidate. (0-origin)
     * @return true if it takes valid index.
     */
    virtual bool resetCandidate (size_t index) = 0;

    /**
     * \brief Unselects a selected text.
     * @return true if there are some selected text.
     */
    virtual bool unselectCandidates () = 0;

    /**
     * \brief Checks the candidate is exist or not.
     * @param index Index of the candidate. (0-origin)
     * @return true if the candidate is exist.
     * @see getPreparedCandidatesSize
     *
     * This method may update prepared
     * candidates size.
     */
    virtual bool hasCandidate (size_t index) = 0;

    /**
     * \brief Gets the candidate if exists.
     * @param index Index of the candidate. (0-origin)
     * @param output A candidate which is got.
     * @return true if the candidate is exist.
     * @see getPreparedCandidatesSize
     *
     * This method may update prepared
     * candidates size.
     */
    virtual bool getCandidate (size_t index, Candidate & output) = 0;

    /**
     * \brief Gets a already prepared candidates size.
     * @return Prepared candidates size.
     *
     * To avoid a performance issue, this library prepares candidates on demand.
     * This method returns a size of candidates which are already prepared.
     */
    virtual size_t getPreparedCandidatesSize () const = 0;

    /**
     * \brief Initializes a InputContext class.
     *
     * This is a wrapper function of input (user_cache_dir, user_config_dir).
     * Default values are set to user_cache_dir and user_config_dir.
     * You should call this function at first.
     */
    static void init ();

    /**
     * \brief Initializes a InputContext class.
     * @param user_cache_dir Directory which stores a user cache data.
     *        (input history, etc.)
     * @param user_config_dir Directory which stores a user config data.
     *        If you want to use original special phrase table, please create
     *        "phrases.txt" under this directory.
     *
     * Specifies a directory to stores user data.
     * You can set a same directory to user_cache_dir and user_config_dir.
     * You should call this function at first.
     */
    static void init (const std::string & user_cache_dir,
                      const std::string & user_config_dir);

    /**
     * \brief Finalizes a InputContext class.
     *
     *  You should call it at last.
     */
    static void finalize ();

    /**
     * \brief Creates a new InputContext instance.
     * @param type The type of the input.
     * @param observer Observer to get a notification from the InputContext
     *        instance.
     * @return instance of the InputContext.
     *
     * You should take responsibility for deleting the instance.
     */
    static InputContext * create (InputContext::InputType type,
                                  InputContext::Observer * observer);

    /**
     * \brief Returns a input text.
     * @return input text.
     */
    virtual const std::string & inputText () const = 0;

    /**
     * \brief Returns a selected text.
     * @return selected text.
     */
    virtual const std::string & selectedText (void) const = 0;

    /**
     * \brief Returns a conversion text.
     * @return conversion text.
     */
    virtual const std::string & conversionText (void) const = 0;

    /**
     * \brief Returns a rest text.
     * @return rest text.
     */
    virtual const std::string & restText (void) const = 0;

    /**
     * \brief Returns a auxiliary text.
     * @return auxiliary text.
     */
    virtual const std::string & auxiliaryText (void) const = 0;

    /**
     * \brief Returns a cursor position from the beginning of the input text.
     * @return cursor position.
     */
    virtual unsigned int cursor () const = 0;

    /**
     * \brief Returns a index of the focused candidate.
     * @return index of the focused candidate. (0-origin)
     */
    virtual unsigned int focusedCandidate () const = 0;

    /**
     * \brief Gets property of the context.
     * @param name you want to get.
     * @return value of the property.
     */
    virtual Variant getProperty (PropertyName name) const = 0;

    /**
     * \brief Sets property of the context.
     * @param name you want to get.
     * @param variant value of the property.
     * @return true if the value is set successfully.
     */
    virtual bool setProperty (PropertyName name, const Variant &variant)= 0;
};

}; // namespace PyZy

#endif  // __PYZY_INPUT_CONTEXT_H_
