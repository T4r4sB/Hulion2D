#pragma once
#include "pool.h"

namespace tblib
{
	// поебавшись с шаблонами, € мог бы, возможно, вху€рить ссылки на X:xclass<T> и Y:yclass<T>, где X и Y - параметры шаблонного класса T

	template <int POOL_SIZE> 
	class list2d
	{		
	public :

		struct xclass;
		struct yclass;	

		struct xynode
		{
			int index;
			xclass *x;
			yclass *y;
			xynode *prevx, *nextx, *prevy, *nexty;
			xynode() {}
		};

		struct xclass
		{
			xynode *firsty, *lasty;
			list2d *l;

			xclass () : firsty(NULL), lasty(NULL), l(NULL) {}
			~xclass () { if(l) l->del_x_from_list(this); }
		private :
			xclass (const xclass&);
			xclass& operator = (const xclass&);
		};
		
		struct yclass
		{
			xynode *firstx, *lastx;
			list2d *l;

			yclass () : firstx(NULL), lastx(NULL), l(NULL) {}
			~yclass () { if(l) l->del_y_from_list(this); }
		private :
			yclass (const yclass&);
			yclass& operator = (const yclass&);
		};

	private :
		pool<xynode, POOL_SIZE> nodes;	

	public :
		~list2d()
		{
			for (int i=0; i<nodes.size(); ++i) if (nodes.valid(i))
			{
				new (nodes[i].x) xclass();
				new (nodes[i].y) yclass();
			}
		}

		void put_x_to_y (xclass* x, yclass* y)
		{
			assert (x->l == NULL || x->l == this);
			x->l = this;
			assert (y->l == NULL || y->l == this);
			y->l = this;

			int index = nodes.push();
			xynode* node = &nodes[index];
			node->index = index;
			node->x = x;
			node->y = y;

			node->nexty = NULL;
			node->prevy = x->lasty;
			(x->firsty ? x->lasty->nexty : x->firsty) = node;
			x->lasty = node;
			
			node->nextx = NULL;
			node->prevx = y->lastx;
			(y->firstx ? y->lastx->nextx : y->firstx) = node;
			y->lastx = node;		
		}

		void del_node_from_list (xynode* node)
		{
			xynode* &fromprevx = node->prevx ? node->prevx->nextx : node->y->firstx;
			xynode* &fromnextx = node->nextx ? node->nextx->prevx : node->y->lastx;			
			xynode* &fromprevy = node->prevy ? node->prevy->nexty : node->x->firsty;
			xynode* &fromnexty = node->nexty ? node->nexty->prevy : node->x->lasty;
			assert(fromprevx == node);
			assert(fromnextx == node);
			assert(fromprevy == node);
			assert(fromnexty == node);
			fromprevx = node->nextx;
			fromnextx = node->prevx;
			fromprevy = node->nexty;
			fromnexty = node->prevy;
			nodes.free_index(node->index);
		}

		void del_x_from_first_y (xclass* x)
		{
			xynode* node = x->firsty;
			assert(node);
			del_node_from_list(node);
		}

		void del_x_from_list (xclass* x)
		{
			while (x->firsty)
				del_x_from_first_y(x);
			x->l = NULL;
		}
		
		void del_y_from_first_x (yclass* y)
		{
			xynode* node = y->firstx;
			assert(node);
			del_node_from_list(node);
		}

		void del_y_from_list (yclass* y)
		{
			while (y->firstx)
				del_y_from_first_x(y);
			y->l = NULL;
		}
    
	};
};