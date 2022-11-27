#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "leak_detector_c.h"

//Struct for the assignment
struct item_node_struct {
    char name[32];
    int count;
    struct item_node_struct *left, *right;
};
typedef struct item_node_struct item_node;
struct tree_name_node_struct{
    char treeName[32];
    struct tree_name_node_struct *left, *right;
    item_node *theTree;
};
typedef struct tree_name_node_struct tree_name_node;
//Mallocs for new tree node
tree_name_node *create_tree_node(char *name){
    tree_name_node *tree_node = malloc(sizeof(tree_name_node));

    strcpy(tree_node->treeName, name);
    tree_node->left = NULL;
    tree_node->right = NULL;
    tree_node->theTree = NULL;

    return tree_node;
}
//Mallocs for new item node
item_node *create_item_node(char *name, int val){
    item_node *new_item_node = malloc(sizeof(item_node));

    strcpy(new_item_node->name, name);
    new_item_node->count = val;
    new_item_node->left = NULL;
    new_item_node->right = NULL;
    return new_item_node;
}
//Inserts new tree node
tree_name_node *insert_tree_node(tree_name_node *new_tree_node, char *name){
    if(!new_tree_node)
        return create_tree_node(name);

    if(strcmp(name, new_tree_node->treeName) > 0){
        if(!new_tree_node->right){
            new_tree_node->right = create_tree_node(name);
            return new_tree_node;
        }
        else{
            new_tree_node = new_tree_node->right;
            insert_tree_node(new_tree_node, name);
        }
    }
    else if (strcmp(name, new_tree_node->treeName) < 0){
        if(!new_tree_node->left){
            new_tree_node->left = create_tree_node(name);
            return new_tree_node;
        }
        else {
            new_tree_node = new_tree_node->left;
            insert_tree_node(new_tree_node, name);
        }
    }
    return new_tree_node;
}
//Inserts new item node
item_node *insert_item_node(item_node *new_item_node, char *name, int val){
    if(!new_item_node)
        return create_item_node(name, val);

    if(strcmp(name, new_item_node->name) > 0){

        if(!new_item_node->right){
            new_item_node->right = create_item_node(name, val);
            return new_item_node;
        }
        else{
            new_item_node = new_item_node->right;
            insert_item_node(new_item_node, name, val);
        }
    }
    else if (strcmp(name, new_item_node->name) < 0){
        if(!new_item_node->left){
            new_item_node->left = create_item_node(name, val);
            return new_item_node;
        }
        else {
            new_item_node = new_item_node->left;
            insert_item_node(new_item_node, name, val);
        }
    }
    return new_item_node;
}
//Searches exsisting tree for desired tree
tree_name_node *search_tree(tree_name_node *tree_node, char *family){
    if(!tree_node) return NULL;

    if(strcmp(tree_node->treeName, family) == 0)
        return tree_node;

    if(strcmp(tree_node->treeName, family) < 0 )
        return search_tree(tree_node->right, family);
    else
        return search_tree(tree_node->left, family);
}
//Fills the top tree
tree_name_node *fill_top_half(tree_name_node *root, int n_trees, FILE *ifp){
    char name[32];

    for(int i = 1; i <= n_trees; ++i){
         fscanf(ifp, "%s", name);
        if(!root)
            root = insert_tree_node(root, name);
        else
            insert_tree_node(root, name);
    }
    return root;
}
//Fills the sub-tree
void fill_bottom_half(tree_name_node *root, int n_trees, int n_items, FILE *ifp){
    int count;
    char family[32], species[32];

    for(int i = 1; i <= n_items; ++i){
        fscanf(ifp, "%s %s %d", family, species, &count);
        tree_name_node *tree_node = search_tree(root, family);
        if(!tree_node->theTree)
            tree_node->theTree = insert_item_node(tree_node->theTree, species, count);
        else
            insert_item_node(tree_node->theTree, species, count);
    }
}
//Funnction that manages the creation of the whole tree
tree_name_node *build_tree(tree_name_node *root, int n_trees, int n_items, FILE *ifp){
    root = NULL;
    root = fill_top_half(root, n_trees, ifp);
    fill_bottom_half(root, n_trees, n_items, ifp);
    return root;
}
//Searches sub-tree for for desired item
item_node *search_item(tree_name_node *tree, item_node *item, char *item_name){

    if(!(tree)) return NULL;
    if(!item) return NULL;

    if(strcmp(item->name, item_name) == 0)
        return item;

    else if(strcmp(item->name, item_name) < 0){
            if(!(item->right)) return NULL;
            else
                return search_item(tree, item->right, item_name);
    }
    else{
            if(!(item->left)) return NULL;
            else
                return search_item(tree, item->left, item_name);
    }
}
//prints the subtree inorder
void print_item_inorder(item_node *item_node, FILE *ofp){
    if(!item_node)
        return;

    print_item_inorder( item_node->left, ofp);
    fprintf(ofp,"%s ", item_node->name);
    print_item_inorder(item_node->right, ofp);
}
//prints the whole tree
void print_tree_inorder(tree_name_node *tree, FILE* ofp){
    if(!tree) return;

    print_tree_inorder(tree->left, ofp);
    fprintf(ofp,"\n===%s===\n", tree->treeName);
    if(tree->theTree)
        print_item_inorder(tree->theTree, ofp);
    print_tree_inorder(tree->right, ofp);
}
//prints top tree inorder
void print_top_tree_inorder(tree_name_node *tree, FILE *ofp){
    if(!tree) return;

    print_top_tree_inorder(tree->left, ofp);
    fprintf(ofp,"%s " ,tree->treeName);
    print_top_tree_inorder(tree->right, ofp);
}
//traverses subtree either calculates total count or items before
item_node *item_traversal(item_node *item, char *item_name, int *items_before, int *count, int num){
    if(num == 0){
        if(!item) return NULL;
        if(strcmp(item_name, item->name) > 0)
            ++(*items_before);
        item_traversal(item->left, item_name, items_before, count, 0);
        item_traversal(item->right, item_name, items_before, count,  0);
        return NULL;

    }
    else if (num == 1){
        if(!(item)){
            return NULL;
        }
        (*count) += item->count;
        item_traversal(item->left, item_name, items_before, count, 1);
        item_traversal(item->right, item_name, items_before, count, 1);
        return NULL;
    }
    else return NULL;
}
//gets the height of desired tree
int height (item_node *item){
    if(!item) return -1;
    int l = height(item->left) + 1;
    int r = height(item->right) + 1;
    if(l > r)
        return l;
    else
        return r;
}
//Gets the left-most right child of item to be deleted
item_node *find_lmrc(item_node *node){
    item_node *new_root = node;

    while(new_root && new_root->left)
        new_root = new_root->left;
    return new_root;
}
//Deletes a node
item_node *delete_node(item_node *root, char *item){
    if (!root) return root;

    if(strcmp(root->name, item) > 0 )
        root->left = delete_node(root->left, item);
    else if (strcmp(root->name, item) < 0 )
        root->right = delete_node(root->right, item);
    else {
        if(!(root->left)){
            item_node *temp = root->right;
            free(root);
            return temp;
        }
        else if(!(root->right)){
            item_node *temp = root->left;
            free(root);
            return temp;
        }
        item_node *lmrc = find_lmrc(root->right);
        strcpy(root->name, lmrc->name);
        root->count = lmrc->count;
        root->right = delete_node(root->right, lmrc->name);
    }
    return root;
}
//Deletes the whole item tree postorder
void delete_item_tree(item_node *root){
    if(!root) return;

    delete_item_tree(root->left);
    delete_item_tree(root->right);
    free(root);
}
//Gets the left-most right child of tree to be deleted
tree_name_node *find_tree_lmrc(tree_name_node *node){
    tree_name_node *new_root = node;

    while(new_root && new_root->left)
        new_root = new_root->left;
    return new_root;
}
//Deletes a tree
tree_name_node *delete_tree(tree_name_node *root, tree_name_node *tree){
    if (!root) return root;

    if(strcmp(root->treeName, tree->treeName) > 0 )
        root->left = delete_tree(root->left,  tree);
    else if (strcmp(root->treeName,  tree->treeName) < 0 )
        root->right = delete_tree(root->right,  tree);
    else {
        if(!(root->left)){
            tree_name_node *temp = root->right;
            if(root->theTree)
                delete_item_tree(root->theTree);
            root->theTree = NULL;
            free(root);
            return temp;
        }
        else if(!(root->right)){
            tree_name_node *temp = root->left;
            if(root->theTree)
                delete_item_tree(root->theTree);
            root->theTree = NULL;
            free(root);
            return temp;
        }
        tree_name_node *lmrc = find_tree_lmrc(root->right);
        strcpy(root->treeName, lmrc->treeName);
        root->right = delete_tree(root->right, lmrc);
    }
    return root;
}
//Deletes the whole tree postorder
tree_name_node *delete_the_tree(tree_name_node *root){
    if(!root) return root;

    delete_the_tree(root->left);
    delete_the_tree(root->right);
    if(root->theTree){
        delete_item_tree(root->theTree);
        root->theTree = NULL;
    }
    free(root);
    return root;
}
//Manages deletion of the whole tree
void delete_whole_tree(tree_name_node **root){
    delete_the_tree(*root);
    *root = NULL;
}
//Manages the commands from the output file
void process_commands(FILE *ifp, FILE *ofp, int n_commands, tree_name_node *tree){
    char command[32], tree_name[32], item_name[32];
    int l_height = 0, r_height = 0, items_before = 0, difference = 0;
    fprintf(ofp,"\n====Processing Commands====\n");
    for (int i = 1; i <= n_commands; ++i){
        int tot_count = 0;
        fscanf(ifp, "%s", command);
        if(strcmp(command, "search") == 0){
            fscanf(ifp, "%s %s", tree_name, item_name);
            tree_name_node *tree_node = search_tree(tree, tree_name);
            if(tree_node){
                item_node *item = search_item(tree_node, tree_node->theTree, item_name);
                if(!item)
                    fprintf(ofp,"%s not found in %s\n", item_name, tree_node->treeName);
                else
                    fprintf(ofp,"%d %s found in %s\n",item->count, item->name, tree_node->treeName);
            }
            else fprintf(ofp,"%s does not exist\n", tree_name);
        }
        else if(strcmp(command, "item_before") == 0){
            fscanf(ifp, "%s %s", tree_name, item_name);
            tree_name_node *tree_node = search_tree(tree, tree_name);
            if(tree_node){
                item_traversal(tree_node->theTree, item_name, &items_before, &tot_count, 0);
                fprintf(ofp,"There are %d items before %s\n", items_before, item_name);
            }
        }
        else if(strcmp(command, "height_balance") == 0){
            char balance[15] = "balanced";
            fscanf(ifp, "%s", tree_name);
            tree_name_node *tree_node = search_tree(tree, tree_name);
            if(tree_node){
                l_height = height(tree_node->theTree->left);
                r_height = height(tree_node->theTree->right);
                difference = abs(l_height - r_height);
                if(difference > 1)
                    strcpy(balance , "not balanced");
                fprintf(ofp,"%s: left height %d, right height %d, difference %d, %s\n", tree_node->treeName, l_height, r_height, difference, balance);
            }
        }
        else if(strcmp(command, "count") == 0){
            fscanf(ifp, "%s", tree_name);
            tree_name_node *tree_node = search_tree(tree, tree_name);
            if(tree_node){
                item_traversal(tree_node->theTree, item_name, &items_before, &tot_count, 1);
                fprintf(ofp,"%s count %d\n", tree_node->treeName, tot_count);
            }
        }
        else if(strcmp(command, "delete") == 0){
            fscanf(ifp, "%s %s", tree_name, item_name);
            tree_name_node *tree_node = search_tree(tree, tree_name);
            if(tree_node){
                tree_node->theTree = delete_node(tree_node->theTree, item_name);
                fprintf(ofp,"%s deleted from %s\n", item_name, tree_node->treeName);
            }
        }
       else if(strcmp(command, "delete_tree") == 0){
           fscanf(ifp, "%s", tree_name);
           tree_name_node *tree_node = search_tree(tree, tree_name);
           if(tree_node)
               tree = delete_tree(tree, tree_node);
           fprintf(ofp,"%s deleted\n", tree_name);
       }
        else{
            return;
        }
    }
}

int main (void){
    atexit(report_mem_leak);
    FILE *ifp = fopen("cop3502-as4-input.txt", "r"), *ofp = fopen("cop3502-as4-output-kostyun-jose.txt", "w");

    int n_trees, n_items, n_commands;
    fscanf(ifp, "%d %d %d", &n_trees, &n_items, &n_commands);
    tree_name_node *root = build_tree(root, n_trees, n_items, ifp);

    print_top_tree_inorder(root, ofp);
    print_tree_inorder(root, ofp);
    process_commands(ifp, ofp, n_commands, root);

    delete_whole_tree(&root);

    fclose(ifp);
    fclose(ofp);
    return 0;
}
