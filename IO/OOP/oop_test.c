#include "oop_test.h"

#include <stdio.h>
#include <string.h>

#include "lw_oopc.h"

INTERFACE(IMoveable)
{
    void (*move)(IMoveable* t);
};

ABS_CLASS(Animal)
{
    char name[32];
    int age;

    void (*set_name)(Animal* t, const char* name);
    void (*set_age)(Animal* t, int age);
    void (*say_hello)(Animal* t);
    void (*eat)(Animal* t);
    void (*breathe)(Animal* t);
    void (*init)(Animal* t, const char* name, int age);
};

CLASS(Fish)
{
    EXTENDS(Animal);
    IMPLEMENTS(IMoveable);
    void (*init)(Fish* t, const char* name, int age);
};

CLASS(Dog)
{
    EXTENDS(Animal);
    IMPLEMENTS(IMoveable);
    void (*init)(Dog* t, const char* name, int age);
};

CLASS(Car)
{
    IMPLEMENTS(IMoveable);
};

static void Animal_set_name(Animal* t, const char* name)
{
    strncpy(t->name, name, sizeof(t->name) - 1u);
    t->name[sizeof(t->name) - 1u] = '\0';
}

static void Animal_set_age(Animal* t, int age)
{
    t->age = age;
}

static void Animal_say_hello(Animal* t)
{
    printf("[Animal] name=%s age=%d\r\n", t->name, t->age);
}

static void Animal_init(Animal* t, const char* name, int age)
{
    t->set_name(t, name);
    t->set_age(t, age);
}

ABS_CTOR(Animal)
FUNCTION_SETTING(set_name, Animal_set_name);
FUNCTION_SETTING(set_age, Animal_set_age);
FUNCTION_SETTING(say_hello, Animal_say_hello);
FUNCTION_SETTING(init, Animal_init);
END_ABS_CTOR

DTOR(Animal)
return lw_oopc_true;
END_DTOR

static void Fish_eat(Animal* t)
{
    (void)t;
    printf("[Fish] eat algae\r\n");
}

static void Fish_breathe(Animal* t)
{
    (void)t;
    printf("[Fish] breathe with gill\r\n");
}

static void Fish_move(IMoveable* t)
{
    (void)t;
    printf("[Fish] swim\r\n");
}

static void Fish_init(Fish* t, const char* name, int age)
{
    Animal* base = SUPER_PTR(t, Animal);
    base->set_name(base, name);
    base->set_age(base, age);
}

CTOR(Fish)
SUPER_CTOR(Animal);
FUNCTION_SETTING(Animal.eat, Fish_eat);
FUNCTION_SETTING(Animal.breathe, Fish_breathe);
FUNCTION_SETTING(IMoveable.move, Fish_move);
FUNCTION_SETTING(init, Fish_init);
END_CTOR

DTOR(Fish)
return Animal_dtor(SUPER_PTR(cthis, Animal));
END_DTOR

static void Dog_eat(Animal* t)
{
    (void)t;
    printf("[Dog] eat bone\r\n");
}

static void Dog_breathe(Animal* t)
{
    (void)t;
    printf("[Dog] breathe with lung\r\n");
}

static void Dog_move(IMoveable* t)
{
    (void)t;
    printf("[Dog] run\r\n");
}

static void Dog_init(Dog* t, const char* name, int age)
{
    Animal* base = SUPER_PTR(t, Animal);
    base->set_name(base, name);
    base->set_age(base, age);
}

CTOR(Dog)
SUPER_CTOR(Animal);
FUNCTION_SETTING(Animal.eat, Dog_eat);
FUNCTION_SETTING(Animal.breathe, Dog_breathe);
FUNCTION_SETTING(IMoveable.move, Dog_move);
FUNCTION_SETTING(init, Dog_init);
END_CTOR

DTOR(Dog)
return Animal_dtor(SUPER_PTR(cthis, Animal));
END_DTOR

static void Car_move(IMoveable* t)
{
    (void)t;
    printf("[Car] drive\r\n");
}

CTOR(Car)
FUNCTION_SETTING(IMoveable.move, Car_move);
END_CTOR

DTOR(Car)
return lw_oopc_true;
END_DTOR

ABS_CLASS(OExprNode)
{
    int use;
    void (*print)(OExprNode* t);
    void (*finalize)(OExprNode* t);
};

CLASS(OExpr)
{
    int use;
    OExprNode* root;

    void (*init_int)(OExpr* t, int value);
    void (*init_binary)(OExpr* t, const char* op, OExpr* left, OExpr* right);
    void (*print)(OExpr* t);
};

CLASS(OIntNode)
{
    EXTENDS(OExprNode);
    int value;
    void (*init)(OIntNode* t, int value);
};

CLASS(OBinaryNode)
{
    EXTENDS(OExprNode);
    char op[4];
    OExpr* left;
    OExpr* right;
    void (*init)(OBinaryNode* t, const char* op, OExpr* left, OExpr* right);
};

ABS_CTOR(OExprNode)
cthis->use = 1;
END_ABS_CTOR

DTOR(OExprNode)
if (--cthis->use == 0)
{
    cthis->finalize(cthis);
    return lw_oopc_true;
}
return lw_oopc_false;
END_DTOR

static void OExpr_init_int(OExpr* t, int value)
{
    OIntNode* node = OIntNode_new(lw_oopc_file_line);
    node->init(node, value);
    t->root = SUPER_PTR(node, OExprNode);
}

static void OExpr_init_binary(OExpr* t, const char* op, OExpr* left, OExpr* right)
{
    OBinaryNode* node = OBinaryNode_new(lw_oopc_file_line);
    node->init(node, op, left, right);
    t->root = SUPER_PTR(node, OExprNode);
}

static void OExpr_print(OExpr* t)
{
    t->root->print(t->root);
}

CTOR(OExpr)
FUNCTION_SETTING(init_int, OExpr_init_int);
FUNCTION_SETTING(init_binary, OExpr_init_binary);
FUNCTION_SETTING(print, OExpr_print);
cthis->use = 1;
cthis->root = 0;
END_CTOR

DTOR(OExpr)
if (--cthis->use == 0)
{
    if (cthis->root != 0)
    {
        OExprNode_delete(cthis->root);
    }
    return lw_oopc_true;
}
return lw_oopc_false;
END_DTOR

static void OIntNode_init(OIntNode* t, int value)
{
    t->value = value;
}

static void OIntNode_print(OExprNode* t)
{
    OIntNode* node = SUB_PTR(t, OExprNode, OIntNode);
    printf("%d", node->value);
}

static void OIntNode_finalize(OExprNode* t)
{
    (void)t;
}

CTOR(OIntNode)
SUPER_CTOR(OExprNode);
FUNCTION_SETTING(init, OIntNode_init);
FUNCTION_SETTING(OExprNode.print, OIntNode_print);
FUNCTION_SETTING(OExprNode.finalize, OIntNode_finalize);
END_CTOR

DTOR(OIntNode)
return OExprNode_dtor(SUPER_PTR(cthis, OExprNode));
END_DTOR

static void oexpr_set_op(char* dst, const char* src)
{
    memset(dst, 0, 4);
    strncpy(dst, src, 3);
}

static void OBinaryNode_init(OBinaryNode* t, const char* op, OExpr* left, OExpr* right)
{
    oexpr_set_op(t->op, op);
    t->left = left;
    t->right = right;
    ++left->use;
    ++right->use;
}

static void OBinaryNode_print(OExprNode* t)
{
    OBinaryNode* node = SUB_PTR(t, OExprNode, OBinaryNode);

    printf("(");
    node->left->print(node->left);
    printf("%s", node->op);
    node->right->print(node->right);
    printf(")");
}

static void OBinaryNode_finalize(OExprNode* t)
{
    OBinaryNode* node = SUB_PTR(t, OExprNode, OBinaryNode);
    OExpr_delete(node->left);
    OExpr_delete(node->right);
}

CTOR(OBinaryNode)
SUPER_CTOR(OExprNode);
FUNCTION_SETTING(init, OBinaryNode_init);
FUNCTION_SETTING(OExprNode.print, OBinaryNode_print);
FUNCTION_SETTING(OExprNode.finalize, OBinaryNode_finalize);
END_CTOR

DTOR(OBinaryNode)
return OExprNode_dtor(SUPER_PTR(cthis, OExprNode));
END_DTOR

void oop_test_animal(void)
{
    Fish* fish;
    Dog* dog;
    Car* car;
    Animal* animals[2];
    IMoveable* movers[3];
    int i;

    printf("\r\n[OOP TEST] animal begin\r\n");

    fish = Fish_new(lw_oopc_file_line);
    dog = Dog_new(lw_oopc_file_line);
    car = Car_new(lw_oopc_file_line);

    fish->init(fish, "nemo", 2);
    dog->init(dog, "buddy", 3);

    animals[0] = SUPER_PTR(fish, Animal);
    animals[1] = SUPER_PTR(dog, Animal);
    movers[0] = SUPER_PTR(fish, IMoveable);
    movers[1] = SUPER_PTR(dog, IMoveable);
    movers[2] = SUPER_PTR(car, IMoveable);

    for (i = 0; i < 2; i++)
    {
        animals[i]->say_hello(animals[i]);
        animals[i]->eat(animals[i]);
        animals[i]->breathe(animals[i]);
    }

    for (i = 0; i < 3; i++)
    {
        movers[i]->move(movers[i]);
    }

    Fish_delete(fish);
    Dog_delete(dog);
    Car_delete(car);
    lw_oopc_report();

    printf("[OOP TEST] animal end\r\n");
}

void oop_test_expr(void)
{
    OExpr* left;
    OExpr* right;
    OExpr* sum;
    OExpr* three;
    OExpr* root;

    printf("\r\n[OOP TEST] expr begin\r\n");

    left = OExpr_new(lw_oopc_file_line);
    right = OExpr_new(lw_oopc_file_line);
    sum = OExpr_new(lw_oopc_file_line);
    three = OExpr_new(lw_oopc_file_line);
    root = OExpr_new(lw_oopc_file_line);

    left->init_int(left, 1);
    right->init_int(right, 2);
    sum->init_binary(sum, "+", left, right);

    OExpr_delete(left);
    OExpr_delete(right);

    three->init_int(three, 3);
    root->init_binary(root, "*", sum, three);

    OExpr_delete(sum);
    OExpr_delete(three);

    printf("expr = ");
    root->print(root);
    printf("\r\n");

    OExpr_delete(root);
    lw_oopc_report();

    printf("[OOP TEST] expr end\r\n");
}

void oop_test_all(void)
{
    oop_test_animal();
    oop_test_expr();
}
