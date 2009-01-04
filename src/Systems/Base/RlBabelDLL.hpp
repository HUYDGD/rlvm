// -*- Mode: C++; tab-width:2; indent-tabs-mode: nil; c-basic-offset: 2 -*-
// vi:tw=80:et:ts=2:sts=2
//
// -----------------------------------------------------------------------
//
// This file is part of RLVM, a RealLive virtual machine clone.
//
// -----------------------------------------------------------------------
//
// Copyright (C) 2008 Elliot Glaysher
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
//
// -----------------------------------------------------------------------

#ifndef __RlBabelDLL_hpp__
#define __RlBabelDLL_hpp__

#include "MachineBase/RealLiveDLL.hpp"
#include "MachineBase/reference.hpp"

#include <boost/shared_ptr.hpp>

class TextWindow;

/**
 * Possible commands sent to the rlBabel DLL from the code. These will be
 * passed in as the first integer argument (func) to RlBabelDLL::callDLL().
 */
enum dllFunction {
  dllInitialise       =   0,
  dllTextoutStart     =  10,
  dllTextoutAppend    =  11,
  dllTextoutGetChar   =  12,
  dllTextoutNewScreen =  13,
  dllClearGlosses     =  20,
  dllNewGloss         =  21,
  dllAddGloss         =  22,
  dllTestGlosses      =  23,
  endSetWindowName    =  98,
  endGetCharWinNam    =  99,
  dllSetNameMod       = 100,
  dllGetNameMod       = 101,
  dllSetWindowName    = 102,
  dllGetTextWindow    = 103,
  dllGetRCommandMod   = 104,
  dllMessageBox       = 105,
  dllSelectAdd        = 200
};

// -----------------------------------------------------------------------

/**
 * Return codes from the above functions sent back to the RealLive bytecode.
 */
enum getcReturn {
  getcError       = 0,
  getcEndOfString = 1,
  getcPrintChar   = 2,
  getcNewLine     = 3,
  getcNewScreen   = 4,
  getcSetIndent   = 5,
  getcClearIndent = 6,
  getcBeginGloss  = 7
};

// -----------------------------------------------------------------------

/**
 * rlvm's implementation of the rlBabel "DLL". Handles calls from
 * specially compiled RealLive bytecode to implement the following
 * extra features on top of normal RL bytecode:
 *
 * - Text in codepages other than cp932.
 * - Western text lineation.
 * - "Glosses," a system of simple hyperlinks.
 *
 * @name How rlBabel works internally
 *
 * Games are dissassembled with kprl, their resources are translated,
 * and are recompiled with rlBabel.kh with a special compiler flag and
 * the line "#load 'rlBabel'" at the top of the file. rlBabel.kh
 * redefines several normal functions to be rerouted through a
 * CallDll() call, along with special casing for textout.
 *
 * rlc will compile the disassembled source differently. Instead of
 * the normal textout method, it will add calls which will put the
 * text in a buffer (dllTextoutStart, dllTextoutAppend).
 *
 * Once everything is placed in the buffer, dllTextoutGetChar will be
 * called for a list of actions to take (getcNewLine, getcNewScreen,
 * getcSetIndent, getcPrintChar, etc.) Each character will be pulled
 * out of the buffer (getcPrintChar) and displayed, moving the text
 * insertion point to a location provided by the DLL.
 */
class RlBabelDLL : public RealLiveDLL {
 public:
  RlBabelDLL();

  // Overridden from RealLiveDLL:

  // Main entrypoint to the "DLL". It's a giant switch function that handles
  // all the commands that Haeleth added with rlBabel.
  virtual int callDLL(RLMachine& machine, int func, int arg1, int arg2,
                      int arg3, int arg4);

 private:
  /// Initializes the DLL.
  int initialize(int dllno, int windname);

  /// Takes an input string and copies it to our internal buffer.
  int textoutAdd(RLMachine& machine, const std::string& str);

  /// Adds characters to the internal buffer, italicizing text as it comes in.
  void AppendChar(const char*& ch);

  /// Clears our intenrnal text buffer.
  void textoutClear();

  // Checks if there's room on this page, and either line breaks (returns
  // getcNewLine) or page breaks (returns getcNewScreen).
  int textoutLineBreak(RLMachine& machine, StringReferenceIterator buf);

  /// Retrieves an action specified in getcReturn, which directs the side of
  /// rlBabel implemented in RealLive code. Uses buffer and xmod as output
  /// variables for the command given.
  int textoutGetChar(RLMachine& machine,
                     StringReferenceIterator buffer,
                     IntReferenceIterator xmod);

  /// (rlBabel function not entirely understood...)
  int startNewScreen(RLMachine& machine, const std::string& cnam);

  /// Sets the window name internally. This does not display the name in the
  /// case of NAME_MOD being 0 (name displayed inline), but will display it in
  /// case of NAME_MOD being 1 (name being displayed in a different window
  /// where it won't mess with our indentation rules.)
  int setCurrentWindowName(RLMachine& machine, StringReferenceIterator buffer);

  // Helper functions:

  int getCharWidth(RLMachine& machine, unsigned short full_char, bool as_xmod);

  bool lineBreakRequired(RLMachine& machine);

  unsigned short consumeNextCharacter(std::string::size_type& index);

  inline char& curPos(int offset = 0) {
    return cp932_text_buffer[text_index + offset];
  }
  inline const char& curPos(int offset = 0) const {
    return cp932_text_buffer[text_index + offset];
  }
  inline char& endToken(int offset = 0) {
    return cp932_text_buffer[end_token_index + offset];
  }
  inline const char& endToken(int offset = 0) const {
    return cp932_text_buffer[end_token_index + offset];
  }

  /// Transform one of rlBabel's integer addresses into an iterator to the
  /// corresponding piece of integer memory.
  IntReferenceIterator getIvar(RLMachine& machine, int addr);

  /// Transform one of rlBabel's integer addresses into an iterator to the
  /// corresponding piece of integer memory.
  StringReferenceIterator getSvar(RLMachine& machine, int addr);

  /// Converts an incoming window id to text window instance. If |id| is a
  /// negative number, returns the current window.
  boost::shared_ptr<TextWindow> getWindow(RLMachine& machine, int id);

  /// Whether text being added is italicized.
  bool add_is_italic;

  /// Internal text buffer to which text is added by dllTextoutStart and
  /// dllTextoutAppend. Neither |text_index| or |cp932_text_buffer| are
  /// iterators or pointers into this strings character backing since I'm
  /// worried about invalidation.
  std::string cp932_text_buffer;

  /// Current position in |cp932_text_buffer|.
  std::string::size_type text_index;

  /// End of the current token being processed in |cp932_text_buffer|.
  std::string::size_type end_token_index;
};  // end of class RlBabelDll

#endif
