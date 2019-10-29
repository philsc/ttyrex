#include "parser.h"

namespace ttyrex::parser {

Iterator::Iterator(int row_index, const ParsedScreen& parsed_screen)
    : row_index_(row_index), parsed_screen_(&parsed_screen) {}

ParsedRow Iterator::operator*() const { return parsed_screen_->GetRow(row_index_); }

}  // namespace ttyrex::parser
