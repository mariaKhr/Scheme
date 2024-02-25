#pragma once

#include <tokenizer.hpp>
#include <object.hpp>

Object* Read(Tokenizer* tokenizer);

Object* ReadList(Tokenizer* tokenizer);
