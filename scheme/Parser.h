#pragma once

#include "Object.h"
#include "Tokenizer.h"

std::shared_ptr<Object> read(Tokenizer* tokenizer);
