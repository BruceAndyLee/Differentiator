#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <cassert>
#include <cctype>
#include <cerrno>

#ifndef DEBUG
#define DEBUGPRINTF(...) printf("\nDEBUG:\n" __VA_ARGS__)
#else
#define DEBUGPRINTF(...)
#endif

enum
{
    MAX_NODE_STR_LEN  = 128,
    MAX_TREE_FILE_LEN = 1024
};

//#define _DEBUG_MODE_

enum NODE_TYPE
{
    TYPE_DEF,
    TYPE_ACT,
    TYPE_VAR,
    TYPE_CONST,
    TYPE_FUNC,
};


enum NODE_PRTS //priority
{
    PR_DEF,
    PR_LOW,
    PR_MID,
    PR_HIG,
    PR_FUNC
};

    class Node
    {
    public:
        Node    ();
        Node    (char* data);
        Node    (const char* data);
        Node    (char actChar);
        Node    (double value);
        Node    (char* data, NODE_TYPE type, NODE_PRTS priority, Node* ancestor);
        ~Node();

        void        printNode();
        Node*       Dup();
        NODE_TYPE   getType();
        NODE_PRTS   getPriority();
        void        act(); 
        int         hasLeftGrandChildren();
        int         hasRightGrandChildren();
        int         isFuncName();

        NODE_TYPE type_;
        NODE_PRTS priority_;
        char*     data_;
        Node*     left_dec_;
        Node*     right_dec_;
        Node*     ancestor_;
    };

    Node::Node():
        type_       (TYPE_DEF),
        priority_   (PR_DEF),
        data_       ((char*) calloc (MAX_NODE_STR_LEN, sizeof(char))),
        left_dec_   (NULL),
        right_dec_  (NULL),
        ancestor_   (NULL)
        {
            if (!data_)
                printf("Warning: created node %p with empty data\n", this);
        }

    Node::Node(char* data):
        type_       (TYPE_DEF),
        priority_   (PR_DEF),
        data_       (NULL),
        left_dec_   (NULL),
        right_dec_  (NULL),
        ancestor_   (NULL)
        {
            if (!data)
                printf("Warning: data ptr is NULL in node %p\n", this);
            else
            {
                data_       = (char*) calloc (MAX_NODE_STR_LEN, sizeof(char));
                if (!data_)
                {
                    printf("error finding memory\n");
                    exit(2);
                }
                if (!strncpy(data_, data, MAX_NODE_STR_LEN)) 
                {
                    printf("Error copying data str in node %p\n", this);
                    exit(1);
                }
                type_       = getType();
                priority_   = getPriority();
            }
        } 

    Node::Node(const char* data):
        type_       (TYPE_DEF),
        priority_   (PR_DEF),
        data_       (NULL),
        left_dec_   (NULL),
        right_dec_  (NULL),
        ancestor_   (NULL)
        {
            if (!data)//strcpy + calloc
                printf("Warning: data ptr is NULL in node %p\n", this);
            else
            {
                data_       = (char*) calloc (MAX_NODE_STR_LEN, sizeof(char));
                if (!data_)
                {
                    printf("error finding memory\n");
                    exit(2);
                }
                if (!strncpy(data_, data, MAX_NODE_STR_LEN)) 
                {
                    printf("Error copying data str in node %p\n", this);
                    exit(1);
                }
                type_       = getType();
                priority_   = getPriority();
            }
        }

    Node::Node(double value):
        type_       (TYPE_DEF),
        priority_   (PR_DEF),
        data_       (NULL),
        left_dec_   (NULL),
        right_dec_  (NULL),
        ancestor_   (NULL)
        {
            data_ = (char*) calloc (MAX_NODE_STR_LEN, sizeof(char));
            if (!data_)
            {
                printf("Error finding memory\n");
                exit(2);
            }
            sprintf(data_, "%lg", value);
            type_		= TYPE_CONST;
            priority_	= getPriority();
        }

    Node::Node(char* data, NODE_TYPE type, NODE_PRTS priority, Node* ancestor):
        type_       (type),           
        priority_   (priority),
        data_       (NULL),
        left_dec_   (NULL),
        right_dec_  (NULL),
        ancestor_   (ancestor)
        {
            data_ = (char*) calloc (MAX_NODE_STR_LEN, sizeof(char));   
            if (!data_)
            {
                printf("Error finding memory\n");
                exit(2);
            } 
            if (!strncpy(data_, data, MAX_NODE_STR_LEN)) 
            {
                printf("Error copying data str in node %p\n", this);
                exit(1);
            }
            type_       = getType();
            priority_   = getPriority();
        }


    Node::Node(char actChar):
        type_       (TYPE_DEF),
        priority_   (PR_DEF),
        data_       (NULL),
        left_dec_   (NULL),
        right_dec_  (NULL),
        ancestor_   (NULL)
        {                       
            //                      actchar      + eoline char
            data_  = (char*) calloc(sizeof(char) + sizeof('\0'), sizeof(char));
            if (!data_)
            {
                printf("Error finding memory\n");
                exit(2);
            }
            data_[0]	= actChar;
            type_		= getType();
            priority_	= getPriority();
        }

    Node::~Node()
    {
        if (data_)
            free(data_);
        data_     = NULL;
        ancestor_ = NULL;
        type_     = TYPE_DEF;
        priority_ = PR_DEF;
    }

    NODE_TYPE Node::getType()
    {
        if (!data_)
            return TYPE_DEF;

        if (data_[0] == '+' || data_[0] == '*' || data_[0] == '/' || data_[0] == '^')
            return TYPE_ACT;

        if (data_[0] == '-' && isdigit(data_[1]))
            return TYPE_CONST;

		if (data_[0] == '-')
			return TYPE_ACT;

        if (isdigit(data_[0]))
            return TYPE_CONST;

        if (isFuncName())
            return TYPE_FUNC;

        if (isalpha(data_[0]))
            return TYPE_VAR;

        return TYPE_DEF;
    }
    
    NODE_PRTS Node::getPriority()
    {
        if (!data_)
            return PR_DEF;
        switch(type_)
        {
            case TYPE_CONST:
            case TYPE_VAR:
                return PR_LOW;
            case TYPE_ACT:
                if (data_[0] == '+' || data_[0] == '-')
                    return PR_LOW;
                if (data_[0] == '*' || data_[0] == '/')
                    return PR_MID;
                if (data_[0] == '^')
                    return PR_HIG;
            case TYPE_FUNC:
                return PR_FUNC;
            case TYPE_DEF:
                return PR_DEF;
            default:
                printf("Node has unknown type\n");
                printNode();
                exit(0);
        }
    }

    void Node::act()
    {
        if (left_dec_ && right_dec_ && 
            left_dec_->data_ && right_dec_->data_)
        {
            printf("entered act func; action '%c'\n", data_[0]);
            double a = 0.0, b = 0.0, res = 0.0;
            printf("decs data: '%s'%x lr '%s'%x\n", 
                    left_dec_->data_,  (unsigned int) *left_dec_->data_, 
                    right_dec_->data_, (unsigned int) *right_dec_->data_ );
            if (sscanf(left_dec_->data_,  "%lg", &a) && 
                sscanf(right_dec_->data_, "%lg", &b))
            {
                printf("got operands %lg %lg\n", a, b);
                switch(*data_)
                {
                    case '+':
                        res += (a + b);
                        break;
                    case '-':
                        res += (a - b);
                        break;
                    case '*':
                        res += (a * b);
                        break;
                    case '/':
                        res += (a / b);
                        break;
                    case '^':
                        res += pow(a, b);
                        break;
                    default:
                        printf("Unknown action; worked at %i line\n", __LINE__);
                        exit(0);
                }

            }
            else
            {
                printf("error reading arguments while counting %p node\n", this);
                exit(3);
            }
            sprintf(data_, "%lg", res);
            priority_ = PR_LOW;
        }
        printf("counted successfully\n");
    }

    Node* Node::Dup()//construc
    {
        Node* newNodePtr = new Node(data_, type_, priority_, ancestor_);

        if (left_dec_)
            newNodePtr->left_dec_  = left_dec_->Dup();

        if (right_dec_)
            newNodePtr->right_dec_ = right_dec_->Dup();

        return newNodePtr;
    }

    void Node::printNode()
    {
        printf("\n");
        printf("Node pointer is %p\n", this);
        if (this)
        {
            if (data_)
                printf("Node data      '%s'\n", data_);
            else
                printf("has no data\n");
    
            if (ancestor_)
            {
                printf("ancestor         ptr is %p\n", ancestor_);
                printf("ancestor's data  %s\n", ancestor_->data_);
            }
            printf("Left descendant  ptr %p\n", left_dec_);
            printf("Right descendant ptr %p\n", right_dec_);
            if (left_dec_)
            {
                printf("left dec data    ptr %p\n", left_dec_->data_);
                printf("left_dec data    %s\n", left_dec_->data_);
            }
            if (right_dec_)
            {
                printf("right dec data   ptr %p\n", right_dec_->data_);
                printf("right_dec data   %s\n", right_dec_->data_);
            }
            printf("Node type is         %i\n", type_);
            printf("\n");
        }
    }

    int Node::isFuncName()
    {   
#define _FUNCTIONS_
#define MATH_FUNC(funcName, notused)    \
        if(!strcmp(#funcName, data_))   \
            return 1;                   
#include "MATH_FUNCTIONS"
#undef MATH_FUNC
#undef _FUNCTIONS_
    return 0;
    }

class Differentator
{
    public:    
        Differentator   (FILE* file_to_read, FILE* res_file, const char* tex_file);
        ~Differentator  ();
        void    delete_subTree   (Node** head);
        void    printTree        (Node* root);
        void    inFilePrint      ();
        void    inFilePrint_dot  (Node* head);
        void    inFilePrint_tex  (Node* head1, Node* head2);
        void    buildTree        ();
        void    alterTree        (Node** curNodePtr);
        void    derivative       ();
        char*   sprintTree       (Node* curNodePtr, char* dest);//const + free
    private:

		Node* GetExprNode();
		Node* GetSumSubNode();
		Node* GetMulDivNode();
		Node* GetPowNode();
		Node* GetBracesNode();
		Node* GetNumberNode();

        Node*	_derivative      (Node* curNodePtr);
        Node*	_nodeDerivMul    (Node* left_node, Node* right_node);
        Node*	_nodeDerivSum    (Node* left_node, Node* right_node);
        Node*	_nodeDerivSub    (Node* left_node, Node* right_node);
        Node*	_nodeDerivDiv    (Node* left_node, Node* right_node);
        Node*	_nodeDerivPow    (Node* left_node, Node* right_node);
        Node*	_nodePow         (Node* left_node, double deg);
        Node*	_buildTree       (Node*  curNodePtr);
        void	_printTree       (Node*  curNodePtr);
        void	_sprintTree      (Node*  curNodePtr, char* tree_str);
        void	_inFilePrint     (Node*  curNodePtr);  
        void	_inFilePrint_dot (Node*  curNodePtr, FILE* gv_f);
        void	_inFilePrint_tex (Node*  curNodePtr);
        void	_countTree       (Node*  curNodePtr);
        void	_zeroMul         (Node*  curNodePtr);
        void	_zeroSum         (Node** curNodePtr);
        void	_zeroSub         (Node** curNodePtr);
        void	_zeroDiv         (Node*  curNodePtr);
        void	_zeroPow         (Node*  curNodePtr);
        void	_unitMul         (Node** curNodePtr);
        void	_unitDiv         (Node** curNodePtr);
        int		_d_equal         (double a, double b);
#define _FUNCTIONS_
#define MATH_FUNC(funcName, notused)                \
        Node*  _##funcName##Der(Node* curNodePtr);  \
        void   _##funcName##Calc(Node* node);
#include "MATH_FUNCTIONS"
#undef  MATH_FUNC
#undef  _FUNCTIONS_

        Node*   root_;
        Node*   new_root_;
        FILE*   file_to_write_;
		FILE*	tx_f;
        char*   expr_;
        int     expr_offset_;
};

Differentator::Differentator(FILE* file_to_read, FILE* res_file, const char* tex_file):
    root_           (NULL),
    new_root_       (NULL),
    file_to_write_  (res_file),
	tx_f			(NULL),
    expr_           (0),
    expr_offset_    (0)
    {
		(tex_file)?tx_f = fopen(tex_file, "w"):tx_f = fopen("MathShit.txt", "w");
	    if (!tx_f)
	    {
	        printf("Differentator: error: cannot open/create .tex file\n");
	        exit(EXIT_FAILURE);
	    }
        unsigned int file_size = 0;
        fseek(file_to_read, 0, SEEK_END);
        file_size = (unsigned int) ftell(file_to_read);
        rewind(file_to_read);
        assert(MAX_TREE_FILE_LEN >= 1024);
        if (file_size > MAX_TREE_FILE_LEN)
        {
            printf("Your expression is too big\nI think you're tryina get my prog down, aren't ya?\n");
            exit(1);
        }
        expr_ = (char*) calloc (file_size, sizeof(char));
        printf("fileSize is %u\n", file_size);
        if (!expr_)
        {
            printf("Cannot find memory to read file\n");
            exit(0);
        }
        if (fscanf(file_to_read, "%s", expr_) == EOF)
		{
			printf("Differentator: Error reading expression file: %s\n", strerror(errno));
			exit(EXIT_FAILURE);
		}
    }

Differentator::~Differentator()
{
    printf("in diff ditor\n");
    delete_subTree(&root_);
    delete_subTree(&new_root_);
    file_to_write_= NULL;
    free(expr_);
    expr_         = NULL;
    expr_offset_  = 0;
}

void Differentator::delete_subTree(Node** head)
{
    if (*head)
    {
        delete_subTree(&(*head)->left_dec_);
        delete_subTree(&(*head)->right_dec_);
        delete *head;
        *head = NULL;
    }
}

/*void Differentator::visitor(Node* node_ptr)
{
        node_ptr->printNode();
    if (node_ptr->left_dec_)
        visitor(node_ptr->left_dec_);
    if (node_ptr->right_dec_)
        visitor(node_ptr->right_dec_);
}*/

void Differentator::buildTree()
{
    //root_ = _buildTree(root_);
	root_ = GetExprNode();
	printTree(root_);
}


/* BUILDING TREE OUT OF INF EXPRESSION */


Node* Differentator::GetExprNode()
{
	 Node* head = GetSumSubNode();

	if (expr_[expr_offset_] == '\0')
		return head;
	return NULL;
}

Node* Differentator::GetSumSubNode()
{               
	Node* l = GetMulDivNode();
	Node* SumSubNode = NULL;
	while (expr_[expr_offset_] == '-' || expr_[expr_offset_] == '+')
	{
		SumSubNode = (expr_[expr_offset_] == '+')?new Node("+"):new Node("-");
		expr_offset_++;
		SumSubNode->right_dec_ = GetMulDivNode();
	}	
	SumSubNode?SumSubNode->left_dec_ = l:SumSubNode = l;
	return SumSubNode;
}

Node* Differentator::GetMulDivNode()
{
	Node* l = GetPowNode();
	Node* MulDivNode = NULL;
	while (expr_[expr_offset_] == '*' || expr_[expr_offset_] == '/')
	{
		MulDivNode = (expr_[expr_offset_] == '*')?new Node("*"):new Node("/");
		expr_offset_++;
		MulDivNode->right_dec_ = GetPowNode();
	}
	MulDivNode?MulDivNode->left_dec_ = l:MulDivNode = l;
	return MulDivNode;
}

Node* Differentator::GetPowNode()
{
	Node* PowNode = NULL;
	while (expr_[expr_offset_] == '^')
	{
		PowNode = new Node("^");
		expr_offset_++;
		PowNode->right_dec_ = GetBracesNode();
	}
	PowNode?PowNode->left_dec_ = GetBracesNode(): PowNode = GetBracesNode();
	return PowNode;
}

Node* Differentator::GetBracesNode()
{
	if (expr_[expr_offset_] == '(')
	{
		expr_offset_++;
		Node* BracesNode = GetSumSubNode();
		if (expr_[expr_offset_] == ')')
		{
			expr_offset_++;
			return BracesNode;
		}
		else
		{
			printf("Something's wrong with your math: closing brace not found\n");
			exit(0);
		}
	}
	else	return GetNumberNode();
}

Node* Differentator::GetNumberNode()
{
	double val = 0;
	int rem_offset = expr_offset_;
	Node* LeafNode = NULL;
	if ( isalpha(expr_[expr_offset_]) )
	{
		char var_name = expr_[expr_offset_];
		LeafNode = new Node(&var_name);
		expr_offset_++;
		return LeafNode;
	}
	int if_fract = 0;
	int fract_counter = 0;
	while ( ('0' <= expr_[expr_offset_] && expr_[expr_offset_] <= '9') || 
					expr_[expr_offset_] == '.')
	{
		if (expr_[expr_offset_] == '.') 
		{
			if_fract = 1;
			expr_offset_++;
			continue;
		}
		if (if_fract) fract_counter++;
		val = val * 10 + (expr_[expr_offset_] - '0');
		expr_offset_++;
	}
	val /= pow(10, fract_counter);
	LeafNode = new Node(val);
	if (rem_offset != expr_offset_) return LeafNode;//if there is at least one digit
	else			return NULL;
}

Node* Differentator::_buildTree(Node* curNodePtr)
{
    int stopCharPtr = 0;
    expr_offset_++;    // move tree ptr to a char that's next to '('
    assert (MAX_NODE_STR_LEN >= 128);
    char* curData = (char*) calloc (MAX_NODE_STR_LEN, sizeof(char));
    if (!curData)
    {
        printf("error finding memory for putting another node data\n");
        exit(2);
    }
    if (sscanf(expr_ + expr_offset_, "%127[^()_]%n", curData, &stopCharPtr))
        switch (expr_[expr_offset_ + stopCharPtr])
        {
            case '(':
                expr_offset_ += stopCharPtr;
                curNodePtr = new Node(curData);
                
                curNodePtr->left_dec_   = _buildTree(curNodePtr->left_dec_);

                curNodePtr->right_dec_  = _buildTree(curNodePtr->right_dec_);
                expr_offset_++; //skipping closing brace
                break;
            case ')': //curnodeptr = null + ret cyrnodtr        
                expr_offset_ += stopCharPtr + 1;
                curNodePtr = new Node(curData);
                break;
            case '_':
                expr_offset_++;
                curNodePtr = NULL;
                break;
            default:
                printf("got unknown ruling char '%c' near %i char; worked at %i line\n", expr_[expr_offset_], expr_offset_ + 1, __LINE__);
                exit(0);
                break;
        }
    else
        printf("error reading expression\n");
    free(curData);
    return curNodePtr;
}

void Differentator::inFilePrint()
{
    _inFilePrint(root_);
}

void Differentator::_inFilePrint(Node* curNodePtr)
{
    fprintf(file_to_write_, "(%s", curNodePtr->data_);
    if(curNodePtr->left_dec_)
        _inFilePrint(curNodePtr->left_dec_);
    if(curNodePtr->right_dec_)
        _inFilePrint(curNodePtr->right_dec_);
    fprintf(file_to_write_,")");
}

void Differentator::inFilePrint_dot(Node* head)
{
    if (head)
    {
        FILE* gv_f = fopen("expr_tree.gv", "w");//
        fprintf(gv_f, "digraph G{\n");
       _inFilePrint_dot(head, gv_f);
        fprintf(gv_f, "\n}");
        fclose(gv_f);
        if (system("xdot expr_tree.gv") == -1)
            printf("inFilePrint_dot: could not run .gv compiling\n");
    
        if (system("dot -Tpng expr_tree.gv -o tree.png") == -1)
            printf("inFilePrint_dot: could not evince dot tree picture\n");
    }
}

void Differentator::_inFilePrint_dot(Node* curNodePtr, FILE* gv_f)
{
    fprintf(gv_f, "_node_%p [label=\"data_ '%s'[%p]\\l"
                            "left_dec_ptr  = %p\\l"
                            "right_dec_ptr = %p\\l"
                            "ancestor_ptr  = %p\\l"
                            "node_type is  = %i\\l"
                            "node priority = %i\\l"
                            "its ptr         %p\"]\n", curNodePtr, 
                                                       curNodePtr->data_, 
                                                       curNodePtr->data_, 
                                                       curNodePtr->left_dec_,
                                                       curNodePtr->right_dec_,
                                                       curNodePtr->ancestor_,
                                                       curNodePtr->type_,
                                                       curNodePtr->priority_,
                                                       curNodePtr);
    if (curNodePtr->left_dec_)
    {
        fprintf(gv_f, "_node_%p -> _node_%p\n", curNodePtr, curNodePtr->left_dec_);
        _inFilePrint_dot(curNodePtr->left_dec_,  gv_f);
    }
    if (curNodePtr->right_dec_)
    {
        fprintf(gv_f, "_node_%p -> _node_%p\n\n", curNodePtr, curNodePtr->right_dec_);
        _inFilePrint_dot(curNodePtr->right_dec_, gv_f);
    }    
}

void Differentator::inFilePrint_tex(Node* head1, Node* head2)
{
    fprintf(tx_f,   "\\documentclass{article}\n"
                    "\\title{\\LaTeX\\ Derivation}\n"
                    "\\author{Arbuce Lee}\n"
                    "\\begin{document}\n"
                    "\n\\maketitle\n\n"
                    "\\fontsize{10}{10pt}\\selectfont\n"
                    "\\begin{equation}\n");

    fprintf(tx_f, "\\left[");
    _inFilePrint_tex(head1);
    fprintf(tx_f, "\\right]'=");

    _inFilePrint_tex(head2);
    fprintf(tx_f, "\n\\end{equation}\n\n\\end{document}");

    fclose(tx_f);
    if (system("pdflatex MathShit.tex") == -1)
        printf("inFilePrint_tex: could not run .tex compiling\n");
    if (system("evince MathShit.pdf")	== -1)
        printf("inFilePrint_tex: could not evince pdf\n");
}

void Differentator::_inFilePrint_tex(Node* curNodePtr)
{
    switch(curNodePtr->type_)
    {
        case TYPE_DEF:
            printf("_inFilePrint_tex: node %p type is not set\n", curNodePtr);
			curNodePtr->printNode();
            exit(4);
        case TYPE_CONST:
            fprintf(tx_f, "%s", curNodePtr->data_);
            break;
        case TYPE_VAR:
            fprintf(tx_f, "%s", curNodePtr->data_);
            break;
        case TYPE_FUNC:
            fprintf(tx_f, "%s\\left({", curNodePtr->data_);
            _inFilePrint_tex(curNodePtr->left_dec_);
            fprintf(tx_f, "}\\right)");
            break;
        case TYPE_ACT:
            switch(curNodePtr->data_[0])
            {
                case '+':
                    fprintf(tx_f, "{");
                    _inFilePrint_tex(curNodePtr->left_dec_);
                    fprintf(tx_f, "}+{");
                    _inFilePrint_tex(curNodePtr->right_dec_);
                    fprintf(tx_f, "}");
                    break;
                case '-':
                    fprintf(tx_f, "{");
                    _inFilePrint_tex(curNodePtr->left_dec_);
                    fprintf(tx_f, "}-{");
                    _inFilePrint_tex(curNodePtr->right_dec_);
                    fprintf(tx_f, "}");
                    break;
                case '*':
                    if (curNodePtr->priority_ > curNodePtr->left_dec_->priority_)
                    {
                        (curNodePtr->left_dec_->type_ == TYPE_ACT)?fprintf(tx_f, "\\left({"):fprintf(tx_f, "{");
                        _inFilePrint_tex(curNodePtr->left_dec_);
                        (curNodePtr->left_dec_->type_ == TYPE_ACT)?fprintf(tx_f, "}\\right)*"):fprintf(tx_f, "}");
                    }
                    else
                    {
                        fprintf(tx_f, "{");
                        _inFilePrint_tex(curNodePtr->left_dec_);
                        fprintf(tx_f, "}");
                    }
                    if (curNodePtr->priority_ > curNodePtr->right_dec_->priority_)
                    {
                        (curNodePtr->right_dec_->type_ == TYPE_ACT)?fprintf(tx_f, "*\\left({"):fprintf(tx_f, "*{");
                        _inFilePrint_tex(curNodePtr->right_dec_);
                        (curNodePtr->right_dec_->type_ == TYPE_ACT)?fprintf(tx_f, "}\\right)"):fprintf(tx_f, "}");
                    }
                    else
                    {
                        fprintf(tx_f, "*{");
                        _inFilePrint_tex(curNodePtr->right_dec_);
                        fprintf(tx_f, "}");
                    }
                    break;
                case '/':
                    fprintf(tx_f, "\\frac{");
                    _inFilePrint_tex(curNodePtr->left_dec_);
                    fprintf(tx_f, "}{");
                    _inFilePrint_tex(curNodePtr->right_dec_);
                    fprintf(tx_f, "}");
                    break;
                case '^':
                    fprintf(tx_f, "{\\left({");
                    _inFilePrint_tex(curNodePtr->left_dec_);
                    (curNodePtr->right_dec_->type_ != TYPE_CONST)?fprintf(tx_f, "}\\right)}^{\\left({"):fprintf(tx_f, "}\\right)}^{");
                    _inFilePrint_tex(curNodePtr->right_dec_);
                    (curNodePtr->right_dec_->type_ != TYPE_CONST)?fprintf(tx_f, "}\\right)}"):fprintf(tx_f, "}");
                    break;
                default:
                    printf("Unknown arythmetic action!\n");
                    exit(2);
            }
            break;
        default:
            printf("_inFilePrint_tex: got unknown node type %p\n", curNodePtr);
            exit(4);
    }
}

void Differentator::printTree(Node* root)
{
    printf("\n");
    _printTree(root);
    printf("\n");
}

void Differentator::_printTree(Node* curNodePtr)
{
    printf("(");
	if (curNodePtr)
	{
	    if (curNodePtr->data_)
	        printf("%s", curNodePtr->data_);
    
		if (curNodePtr->left_dec_)
	        _printTree(curNodePtr->left_dec_);

		if (curNodePtr->right_dec_)
			_printTree(curNodePtr->right_dec_);
	}
    printf(")");
}

char* Differentator::sprintTree(Node *curNodePtr, char* dest)
{
    if (!dest)
    {
        printf("sprintTree: Got invalid str ptr\n");
        return NULL;
    }
    strcpy(dest, "");
    _sprintTree(curNodePtr, dest);
    return dest;
}

void Differentator::_sprintTree(Node* curNodePtr, char* tree_str)
{
    strcat(tree_str, "(");
	if (curNodePtr)
	{
	    if (curNodePtr->data_)
	        strcat(tree_str, curNodePtr->data_);

		if (curNodePtr->left_dec_)
	        _sprintTree(curNodePtr->left_dec_, tree_str);

		if (curNodePtr->right_dec_)
			_sprintTree(curNodePtr->right_dec_, tree_str);
	}
    strcat(tree_str, ")");
}

#define _FUNCTIONS_
#define MATH_FUNC(funcName, cppFuncName)                    \
void Differentator::_##funcName##Calc(Node* node)           \
{                                                           \
    if (node->left_dec_->type_ == TYPE_CONST)               \
    {                                                       \
        double argument = 0.0;                              \
        sscanf(node->left_dec_->data_, "%lg", &argument);   \
        sprintf(node->data_, "%lg", cppFuncName(argument)); \
        delete node->left_dec_;                             \
        node->left_dec_ = NULL;                             \
        node->type_ = TYPE_CONST;                           \
    }                                                       \
}
#include "MATH_FUNCTIONS"
#undef MATH_FUNC
#undef _FUNCTIONS_

void Differentator::alterTree(Node** head)
{
    char* str1     = (char*) calloc (MAX_NODE_STR_LEN, sizeof(char));
    char* tree_str = (char*) calloc (MAX_NODE_STR_LEN, sizeof(char));
    str1 = strcat(str1, "mum soap frame");
    if (!tree_str)
    {
        printf("alterTree: Error finding memory for tree str\n");
        exit(4);
    }
    tree_str = sprintTree(*head, tree_str);
    while (strcmp(str1, tree_str))
    {
		
        printf("IN ALTER TREEE\n");
        _countTree(*head);
		_printTree(*head);	
        _zeroMul(*head);
		_printTree(*head);
        _zeroSum(head);

        _zeroSub(head);

        _zeroPow(*head);

        _unitMul(head);

        _unitDiv(head);

        str1 = strncpy(str1, tree_str, MAX_NODE_STR_LEN);
        tree_str = sprintTree(*head, tree_str);
        printf( "before iteration {%s}\n"
                "after iteration  {%s}\n", str1, tree_str);
    }
    free(tree_str);
    free(str1);
}

void Differentator::_countTree(Node* curNodePtr)
{
    if(curNodePtr->left_dec_)
        _countTree(curNodePtr->left_dec_);

    if(curNodePtr->right_dec_)
        _countTree(curNodePtr->right_dec_);

    if (curNodePtr->left_dec_ && curNodePtr->right_dec_ 
                              && curNodePtr->type_              == TYPE_ACT 
                              && curNodePtr->left_dec_->type_   == TYPE_CONST 
                              && curNodePtr->right_dec_->type_  == TYPE_CONST)
    {
        curNodePtr->act();
        delete curNodePtr->left_dec_;
        delete curNodePtr->right_dec_;
        curNodePtr->left_dec_ = curNodePtr->right_dec_ = NULL;
        curNodePtr->type_ = TYPE_CONST;
    }
    if (curNodePtr->type_ == TYPE_FUNC)
    {
#define _FUNCTIONS_
#define MATH_FUNC(funcName, notused)\
if(!strcmp(curNodePtr->data_, #funcName))\
    _##funcName##Calc(curNodePtr);
#include "MATH_FUNCTIONS"
#undef MATH_FUNC
#undef _FUNCTIONS_
    }
}

int Differentator::_d_equal(double a, double b)
{
    if(fabs(a - b) < 0.00001)
        return 1;
    return 0;
}

void Differentator::_zeroMul(Node* curNodePtr)
{
    if (curNodePtr->data_ && curNodePtr->data_[0] == '*' &&
        curNodePtr->left_dec_ && curNodePtr->right_dec_)
    {
        NODE_TYPE l_type = curNodePtr->left_dec_->type_;
        NODE_TYPE r_type = curNodePtr->right_dec_->type_;
            double a = 1.0, b = 1.0;
            if (l_type == TYPE_CONST)
                sscanf(curNodePtr->left_dec_->data_, "%lg", &a);
            if (r_type == TYPE_CONST)
                sscanf(curNodePtr->right_dec_->data_, "%lg", &b);
    
            if (_d_equal(a, 0.0) || _d_equal(b, 0.0))
            {
                curNodePtr->data_[0]    = '0';
                curNodePtr->type_       = TYPE_CONST;
                curNodePtr->priority_   = PR_LOW,

                delete_subTree(&curNodePtr->left_dec_);
                printf("left one is deleted\n");
                delete_subTree(&curNodePtr->right_dec_);
                curNodePtr->left_dec_ = curNodePtr->right_dec_ = NULL;
                printf("right is deleted\n");
            }
    }
    if (curNodePtr->left_dec_)
        _zeroMul(curNodePtr->left_dec_);
        
    if (curNodePtr->right_dec_)
        _zeroMul(curNodePtr->right_dec_);
}

void Differentator::_zeroPow(Node* curNodePtr)
{
    if (curNodePtr->data_     && curNodePtr->data_[0] == '^' &&
        curNodePtr->left_dec_ && curNodePtr->right_dec_)
    {
            double degree;
            sscanf(curNodePtr->right_dec_->data_, "%lg", &degree);
            if (_d_equal(degree, 0.0))
            {
                curNodePtr->data_[0]    = '1';
                curNodePtr->type_       = TYPE_CONST;
                curNodePtr->priority_   = PR_LOW;
                delete_subTree(&curNodePtr->left_dec_);
                delete_subTree(&curNodePtr->right_dec_);
                curNodePtr->right_dec_  = curNodePtr->left_dec_ = NULL;
            }
    }
    if (curNodePtr->left_dec_)
        _zeroPow(curNodePtr->left_dec_);
    if (curNodePtr->right_dec_)
        _zeroPow(curNodePtr->right_dec_);
}

void Differentator::_unitDiv(Node** curNodePtr)
{
    if ((*curNodePtr)->data_ &&(*curNodePtr)->data_[0] == '/' &&
        (*curNodePtr)->left_dec_ && (*curNodePtr)->right_dec_)
    {
            double r_val = 42.0;
            if ((*curNodePtr)->right_dec_->type_ == TYPE_CONST)
            {
                sscanf((*curNodePtr)->right_dec_->data_, "%lg", &r_val);
                if (_d_equal(r_val, 1.0))
                {
                    Node* node_to_bridge = (*curNodePtr)->left_dec_;
                    delete *curNodePtr;
                    *curNodePtr = node_to_bridge;
                }
            }
    }
    if ((*curNodePtr)->left_dec_)
        _unitDiv( &((*curNodePtr)->left_dec_) );
    if ((*curNodePtr)->right_dec_)
        _unitDiv( &((*curNodePtr)->right_dec_) );
}

#define ZERO_UNIT_NODE_ACTION(funcName, actChar, nodeValue)                 \
void Differentator::funcName(Node** curNodePtr)                             \
{                                                                           \
    if ((*curNodePtr)->data_ && (*curNodePtr)->data_[0] == actChar			\
							 && !isdigit((*curNodePtr)->data_[1]))          \
    {                                                                       \
        if ((*curNodePtr)->left_dec_ && (*curNodePtr)->right_dec_)          \
        {                                                                   \
            printf("IN %s node %p: [%s]\n", #funcName, *curNodePtr, (*curNodePtr)->data_);\
            double l_val = -1.0, r_val = -1.0;                              \
            if ((*curNodePtr)->left_dec_->type_  == TYPE_CONST)             \
                sscanf((*curNodePtr)->left_dec_->data_,  "%lg", &l_val);    \
            if ((*curNodePtr)->right_dec_->type_ == TYPE_CONST)             \
                sscanf((*curNodePtr)->right_dec_->data_, "%lg", &r_val);    \
            if(_d_equal(l_val, nodeValue))                                  \
            {                                                               \
				Node* node_to_bridge = NULL;								\
				if (_d_equal(l_val, 0.0))									\
				{															\
					node_to_bridge = new Node("*");							\
					node_to_bridge->left_dec_= new Node("-1");				\
					node_to_bridge->right_dec_ = (*curNodePtr)->right_dec_->Dup();\
				}															\
				else	node_to_bridge = (*curNodePtr)->right_dec_->Dup();	\
                delete *curNodePtr;                                         \
                *curNodePtr = node_to_bridge;                               \
            }                                                               \
            else                                                            \
                if(_d_equal(r_val, nodeValue))                              \
                {                                                           \
                    Node* node_to_bridge = (*curNodePtr)->left_dec_;        \
                    delete *curNodePtr;                                     \
                    *curNodePtr = node_to_bridge;                           \
                }                                                           \
        }                                                                   \
        else                                                                \
        {                                                                   \
            printf("Arguments for '%c' action requiered\n", actChar);       \
			(*curNodePtr)->printNode();\
            exit(0);                                                        \
        }                                                                   \
    }                                                                       \
    if ((*curNodePtr)->left_dec_)                                           \
        funcName( &((*curNodePtr)->left_dec_) );                            \
    if((*curNodePtr)->right_dec_)                                           \
        funcName( &((*curNodePtr)->right_dec_) );                           \
}
ZERO_UNIT_NODE_ACTION(_unitMul, '*', 1.0)
ZERO_UNIT_NODE_ACTION(_zeroSum, '+', 0.0)
ZERO_UNIT_NODE_ACTION(_zeroSub, '-', 0.0)
#undef ZERO_UNIT_NODE_ACTION

#define NODE_DERIV_SUM_SUB_FUNC(actChar, funcName)              \
Node* Differentator::funcName(Node* left_node, Node* right_node)\
{                                                               \
    Node* new_node       = new Node(actChar);                   \
    new_node->left_dec_  = _derivative(left_node);              \
    new_node->right_dec_ = _derivative(right_node);             \
    new_node->left_dec_->ancestor_  = new_node;                 \
    new_node->right_dec_->ancestor_ = new_node;                 \
    return new_node;                                            \
}
NODE_DERIV_SUM_SUB_FUNC('+', _nodeDerivSum)
NODE_DERIV_SUM_SUB_FUNC('-', _nodeDerivSub)
#undef NODE_SUM_SUB_FUNC

Node* Differentator::_nodeDerivMul(Node* left_dec, Node* right_dec)
{
    Node* new_sum_node   = new Node('+');
    Node* new_l = new Node('*');
    Node* new_r = new Node('*');
    new_l->left_dec_  = _derivative(left_dec);
    new_l->right_dec_ = right_dec->Dup();

    new_r->left_dec_  = left_dec->Dup();
    new_r->right_dec_ = _derivative(right_dec);
    new_sum_node->left_dec_  = new_l;
    new_sum_node->right_dec_ = new_r;
    return new_sum_node;
}

Node* Differentator::_nodeDerivDiv(Node* left_dec, Node* right_dec)
{
    Node* res_node   = new Node('/');
    Node* new_l      = new Node('-');

    Node* new_ll     = new Node('*');
    Node* new_lr     = new Node('*');
    new_ll->left_dec_   = _derivative(left_dec);
    new_ll->right_dec_  = right_dec->Dup();
    new_lr->left_dec_   = left_dec->Dup();
    new_lr->right_dec_  = _derivative(right_dec);

    Node* new_r         = _nodePow(right_dec, 2.0);
    
    new_l->left_dec_  = new_ll;
    new_l->right_dec_ = new_lr;

    res_node->left_dec_  = new_l;
    res_node->right_dec_ = new_r;

    return res_node;
}

Node* Differentator::_nodeDerivPow(Node* left_dec, Node* right_dec)
{
    Node* res_node       = new Node('*');
    res_node->left_dec_  = new Node('*');
    res_node->left_dec_->left_dec_  = new Node(right_dec->data_);
    res_node->left_dec_->right_dec_ = new Node('^');
    res_node->left_dec_->right_dec_->left_dec_ = left_dec->Dup();
    double degree = 0.0;
    if (sscanf(right_dec->data_, "%lg", &degree))
        res_node->left_dec_->right_dec_->right_dec_ = new Node(degree - 1.0);
    else
    {
        printf("error reading degree value\n");
        exit(1);
    }
    res_node->right_dec_ = _derivative(left_dec);
    return res_node;
}

Node* Differentator::_nodePow(Node* left_node, double deg)
{
    Node* pow_node       = new Node('^');
    pow_node->left_dec_  = left_node->Dup();
    pow_node->right_dec_ = new Node(deg);
    return pow_node;
}

#define L_BRANCH curNodePtr->left_dec_
#define R_BRANCH curNodePtr->right_dec_
Node* Differentator::_derivative(Node* curNodePtr)
{
    //printf("in _derivative: curNode %p: [%s]\n", curNodePtr, curNodePtr->data_);
    switch(curNodePtr->type_)
    {
        case TYPE_DEF:
            printf("_derivative: node %p: '%s' type is not set\n", curNodePtr, curNodePtr->data_);
            exit(4);
        case TYPE_CONST:
        {
            return new Node('0');
        }
        case TYPE_VAR:
        {
            return new Node('1');
        }
        case TYPE_ACT:
            switch(curNodePtr->data_[0])
            {
                case '+':
                    return _nodeDerivSum(L_BRANCH, R_BRANCH);
                case '-':
                    return _nodeDerivSub(L_BRANCH, R_BRANCH);
                case '*':
                    return _nodeDerivMul(L_BRANCH, R_BRANCH);
                case '/':
                    return _nodeDerivDiv(L_BRANCH, R_BRANCH);
                case '^':
                {
                    double degree = 0.0;
                    if (sscanf(curNodePtr->right_dec_->data_, "%lg", &degree))
					{
                        if (!_d_equal(degree, 0.0))
                            return _nodeDerivPow(L_BRANCH, R_BRANCH);
					}
                    else
                    {
                        printf("error reading degree val\n");
                        exit(6);
                    }

                }
                default:
                    printf("Cannot distinguish act char; worked at %i line\n", __LINE__);
                    inFilePrint_dot(curNodePtr);
                    curNodePtr->printNode();
                    printf("\n");
                    _printTree(curNodePtr);
                    printf("\n");
                    exit(0);
            }
        case TYPE_FUNC:
#define _FUNCTIONS_
#define MATH_FUNC(funcName, notused)\
            if (!strcmp(curNodePtr->data_, #funcName))\
                return _##funcName##Der(curNodePtr);
#include "MATH_FUNCTIONS"
#undef MATH_FUNC
#undef _FUNCTIONS_
        default:
            printf("Node type is not set or is not recognised; worked at %i line\n", __LINE__);
            curNodePtr->printNode();
            exit(0);

    }
}

Node* Differentator::_lnDer(Node* curNodePtr)
{
    Node* res_node      = new Node('*');
    res_node->left_dec_ = new Node('/');
    res_node->left_dec_->left_dec_  = new Node("1");
    res_node->left_dec_->right_dec_ = curNodePtr->left_dec_->Dup();
    res_node->right_dec_ = _derivative(L_BRANCH);
    return res_node;
}

Node* Differentator::_sinDer(Node* curNodePtr)
{
    Node* res_node = new Node('*');
    Node* new_l = curNodePtr->Dup();
    sprintf(new_l->data_, "cos"); 
    Node* new_r = _derivative(curNodePtr->left_dec_);

    res_node->left_dec_  = new_l;
    res_node->right_dec_ = new_r;
    return res_node;
}

#undef L_BRANCH
#undef R_BRANCH

void Differentator::derivative()
{
    inFilePrint_dot(root_);
    alterTree(&root_);
    inFilePrint_dot(root_);

    printf("BEFORE DERIVATING ORIGIN TREE:  ");
    printTree(root_);
    printf("\n\n");
    new_root_ = _derivative(root_);

    printf("RIGHT AFTER DERIVATING ORIGIN TREE:  ");
    printTree(new_root_);
    printf("\n\n");

    inFilePrint_dot(new_root_);

    printf("BEFORE ALTERING NEW TREE:  ");
    printTree(new_root_);
    printf("\n\n");
    alterTree(&new_root_);

    printf("after ALTERING NEW TREE:  ");
    printTree(new_root_);
    printf("\n\n");
    inFilePrint_dot(root_);
    inFilePrint_dot(new_root_);

    printf("BEFORE FINAL WRITING IN FILE ORIGIN TREE ");
    printTree(root_);
    printf("\n\n");
    inFilePrint_tex(root_, new_root_);
}

int main(int argc, char* argv[])
{
    if (argc < 3)
    {
        printf("Usage: %s [expression_file] [resfile]\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    FILE* f_expr = fopen(argv[1], "r");
    FILE* res_f  = fopen(argv[2], "w");
    if (!f_expr)
    {
        printf("Invalid exprfile name\n");
        exit(0);
    }
    if (!res_f)
    {
        printf("Invalid resfile name\n");
        exit(0);
    }
    Differentator my_diff(f_expr, res_f, "MathShit.tex");

    my_diff.buildTree();
    printf("BUILDED!\n");
    //my_diff.printTree();
    //printf("PRINTED!\n");
    my_diff.derivative(); 
    fclose(f_expr);
    fclose(res_f);
}
