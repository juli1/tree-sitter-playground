// Filename - test-json-parser.c

#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <tree_sitter/api.h>

// Declare the `tree_sitter_json` function, which is
// implemented by the `tree-sitter-json` library.
extern "C" {
extern TSLanguage *tree_sitter_python();
}

int main() {
  // Create a parser.
  TSParser *parser = ts_parser_new();

  // Set the parser's language (JSON in this case).
  ts_parser_set_language(parser, tree_sitter_python());

  // Build a syntax tree based on source code stored in a string.
  const char *source_code = "class Foo:\n  def foo():\n    pass\n  def bar():\n    pass\nclass Bar:\n  def foo():\n    pass\n  def bar():\n    pass\n";
  TSTree *tree = ts_parser_parse_string(
    parser,
    NULL,
    source_code,
    strlen(source_code)
  );

  // Get the root node of the syntax tree.
  TSNode root_node = ts_tree_root_node(tree);

  // Get some child nodes.
  TSNode first_node = ts_node_named_child(root_node, 0);

  // Print the syntax tree as an S-expression.
  char *string = ts_node_string(root_node);
  printf("Syntax tree: %s\n", string);


  const char* query = "(class_definition body: (block (function_definition  name: (identifier) @function_name )+ @function_definition)) @class";
  uint32_t error_offset = 0;
  TSQueryError query_error;
  TSQuery* ts_query = ts_query_new(tree_sitter_python(), query, strlen(query), &error_offset, &query_error);
  printf("error offset = %d\n", error_offset);

  TSQueryCursor* cursor = ts_query_cursor_new();

  ts_query_cursor_exec(cursor, ts_query, root_node);
  TSQueryMatch match;

  uint32_t capture_count = ts_query_capture_count(ts_query);
  printf("capture count: %d\n", capture_count);
  for(uint32_t i = 0 ; i < capture_count ; i++) {
    uint32_t len;

    const char* name = ts_query_capture_name_for_id(ts_query, i, &len);
    printf("%d -> %s\n", i, name);

  }

  while(ts_query_cursor_next_match(cursor, &match)) {
    printf("got a match\n");
    printf("match %d, pattern index = %d, capture count=%d\n", match.id, match.pattern_index, match.capture_count);
    for(int i = 0 ; i < match.capture_count ; i++) {
        TSQueryCapture capture = match.captures[i];
    uint32_t len;

        const char* name = ts_query_capture_name_for_id(ts_query, capture.index, &len);
        printf("capture %d, name: %s, node: %s\n", capture.index, name, ts_node_string(capture.node));
    }
  }

  // Free all of the heap-allocated memory.
  free(string);
  ts_tree_delete(tree);
  ts_parser_delete(parser);
  return 0;
}
