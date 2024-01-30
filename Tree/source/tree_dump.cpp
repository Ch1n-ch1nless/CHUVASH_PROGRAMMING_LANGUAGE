#include "tree_dump.h"

//=================================================================================================

static void NodeGraphDump(Node* node, FILE* dot_file, NameSpace* const name_space)
{
    PTR_ASSERT(dot_file)
    PTR_ASSERT(name_space)

    if (node == nullptr)
    {
        return;
    }

    fprintf(dot_file, "\tnode_%p[label = \"{", node);

    switch (node->data->type)
    {
        case TYPE_PUNCT:
        {
            fprintf("%s | PUNCT | {<left> left | <right> right}}\", fillcolor = \"blue\"];\n", 
                    name_space->array[node->data->value.punctuation]);
            break;
        }

        case TYPE_BIN_OP:
        {
            fprintf("%s | BIN_OP | {<left> left | <right> right}}\", fillcolor = \"blue\"];\n", 
                    name_space->array[node->data->value.bin_op]);
            break;
        }

        case TYPE_UN_OP:
        {
            fprintf("%s | UN_OP | {<left> left | <right> right}}\", fillcolor = \"blue\"];\n", 
                    name_space->array[node->data->value.un_op]);
            break;
        }

        case TYPE_KEY_OP:
        {
            fprintf("%s | KEY_OP | {<left> left | <right> right}}\", fillcolor = \"blue\"];\n", 
                    name_space->array[node->data->value.key_op]);
            break;
        }

        case TYPE_NUMBER:
        {
            fprintf("%lg | NUMBER | {<left> left | <right> right}}\", fillcolor = \"red\"];\n", 
                    node->data->value.number);
            break;
        }

        case TYPE_VARIABLE:
        {
            fprintf("%s | VARIABLE | {<left> left | <right> right}}\", fillcolor = \"yellow\"];\n", 
                    name_space->array[node->data->value.var_index]);
            break;
        }

        case TYPE_FUNCTION:
        {
            fprintf("%s | FUNCTION | {<left> left | <right> right}}\", fillcolor = \"green\"];\n", 
                    name_space->array[node->data->value.func_index]);
            break;
        }

        case TYPE_STRING:
        {
            fprintf("%s | STRING | {<left> left | <right> right}}\", fillcolor = \"purple\"];\n", 
                    node->data->value.string);
            break;
        }

        case TYPE_UNDEFINED:
        default:
            break;
    }

    NodeGraphDump(node->left,  dot_file);

    NodeGraphDump(node->right, dot_file);
}

//=================================================================================================

static void EdgeGraphDump(Node* node, FILE* dot_file)
{
    if (node->left != nullptr)
    {
        fprintf(dot_file, "node_%p -> node_%p [color = \"#000000\"];\n", node, node->left);

        EdgeGraphDump(node->left, dot_file);
    }

    if (node->right != nullptr)
    {
        fprintf(dot_file, "node_%p -> node_%p [color = \"#000000\"];\n", node, node->right);

        EdgeGraphDump(node->right, dot_file);
    }
}

//=================================================================================================

void TreeGraphDump(Tree* const tree, NameSpace* const name_space)
{
    PTR_ASSERT(tree)
    PTR_ASSERT(name_space)

    StrErrors error = STR_ERR_NO;

    FILE* dot_file = nullptr;

    error = OpenFile(DOT_FILE, &dot_file, "w");
    if (error != STR_ERR_NO)
    {
        PrintBufferError(error);
        return;
    }

    //========================================================================================

    fprintf(dot_file, "digraph G\n"
                      "{\n"
                      "\tgraph [dpi = 100];\n\n"
                      "\trankdir = TB;\n\n"
                      "\tedge[minlen = 3, arrowsize = 2, penwidth = 1.5];\n"
                      "\tnode[shape = Mrecord, style = \"rounded, filled\", "
                      "fillcolor = \"yellow\", fontsize = 20, "
                      "penwidth = 3];\n\n");

    //========================================================================================

    NodeGraphDump(tree->root, dot_file);
    fprintf(dot_file, "\n");

    EdgeGraphDump(tree->root, dot_file);
    fprintf(dot_file, "\n");

    //========================================================================================

    fprintf(dot_file, "}\n");

    fclose(dot_file);

    char command[MAX_LEN_OF_WORD] = {};
    sprintf(command, "dot %s -T png -o img_tree_%s.png", DOT_FILE, __TIME__);

    system(command); 

}

//=================================================================================================