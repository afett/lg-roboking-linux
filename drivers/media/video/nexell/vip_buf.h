/*
 * (C) Copyright 2010
 * jung hyun kim, Nexell Co, <jhkim@nexell.co.kr>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
#ifndef __VIP_BUF_H__
#define __VIP_BUF_H__

#include <linux/list.h>

/* nexell video memory */
#include <mach/platform.h>
#include <mach/devices.h>
#include <mach/fourcc.h>
#include <mach/vmem.h>

/*------------------------------------------------------------------------------
 * video memory buffer allocator
 */
struct vm_buffer {
	struct list_head	list;
	VM_VMEMORY			vm;
	int					index;
	int					ref;
};

int 				vip_buf_init(void);
void			 	vip_buf_exit(void);
int				 	vip_buf_alloc(u_int fourcc, u_int width,
								u_int height, int halign, int valign, int count);
void			 	vip_buf_free(void);
struct vm_buffer*	vip_buf_get_vm(int index);
u_int 				vip_buf_get_phys(int index, int field);	/* field: lu=0, cb=1, cr=2 */

/*------------------------------------------------------------------------------
 * buffer manage func
 */
struct buf_head {
	/* isr list */
	struct list_head		i_head;
	struct list_head	*	i_list;
	/* out list */
	struct list_head	 	o_head;
	struct list_head 	*	p_list;		/* preview */
	struct list_head 	*	c_list;		/* capture */
	/* lock */
	spinlock_t 				lock;
};

extern struct buf_head bh_list;

static inline void init_vm_buf_head(struct list_head *head, int count)
{
	struct vm_buffer *vmb;
	int i = 0;

	INIT_LIST_HEAD(&bh_list.i_head);
	INIT_LIST_HEAD(&bh_list.o_head);

	bh_list.i_list = &bh_list.i_head;
	bh_list.p_list = &bh_list.o_head;
	bh_list.c_list = &bh_list.o_head;

	spin_lock_init(&bh_list.lock);

	vmb = container_of(head, struct vm_buffer, list);
	for(; count > i; i++, vmb++) {
		list_add_tail(&vmb->list, &bh_list.i_head);
		vmb->index = i;
		vmb->ref   = 0;
	}
}

static inline struct list_head * get_intr_buf_empty(void)
{
	if (&bh_list.i_head == bh_list.i_list->next)
		return NULL;
	return (bh_list.i_list = bh_list.i_list->next);
}

static inline void set_intr_buf_free(struct list_head *list)
{
	list_del(list);
	list_add_tail(list, &bh_list.o_head);
}

static inline struct list_head * get_intr_buf_next(void)
{
	struct list_head *next;
	if (&bh_list.i_head == bh_list.i_list->next)
		return NULL;

	next = bh_list.i_list->next;

	list_del(bh_list.i_list);
	list_add_tail(bh_list.i_list, &bh_list.o_head);
	bh_list.i_list = next;

	return (next);
}

static inline struct list_head * get_prev_buf_fill(void)
{
	unsigned long flags;
	spin_lock_irqsave(&bh_list.lock, flags);

	if (&bh_list.o_head == bh_list.p_list->next) {
		spin_unlock_irqrestore(&bh_list.lock, flags);
		return NULL;
	}
	bh_list.p_list = bh_list.p_list->next;
	container_of(bh_list.p_list, struct vm_buffer, list)->ref++;

	spin_unlock_irqrestore(&bh_list.lock, flags);
	return bh_list.p_list;
}

static inline void set_prev_buf_free(struct list_head *list)
{
	unsigned long flags;
	spin_lock_irqsave(&bh_list.lock, flags);

	if (--container_of(list, struct vm_buffer, list)->ref)
		goto unlock;

	if (bh_list.p_list == list)
		bh_list.p_list = bh_list.p_list->prev;

	list_del(list);
	list_add_tail(list, &bh_list.i_head);

unlock:
	spin_unlock_irqrestore(&bh_list.lock, flags);
}

static inline struct list_head * get_capt_buf_fill(void)
{
	unsigned long flags;
	spin_lock_irqsave(&bh_list.lock, flags);

	if (&bh_list.o_head == bh_list.c_list->next) {
		spin_unlock_irqrestore(&bh_list.lock, flags);
		return NULL;
	}
	bh_list.c_list = bh_list.c_list->next;
	container_of(bh_list.c_list, struct vm_buffer, list)->ref++;

	spin_unlock_irqrestore(&bh_list.lock, flags);
	return bh_list.c_list;
}

static inline void set_capt_buf_free(struct list_head *list)
{
	unsigned long flags;
	spin_lock_irqsave(&bh_list.lock, flags);

	if (--container_of(list, struct vm_buffer, list)->ref)
		goto unlock;

	if (bh_list.c_list == list)
		bh_list.c_list = bh_list.c_list->prev;

	list_del(list);
	list_add_tail(list, &bh_list.i_head);

unlock:
	spin_unlock_irqrestore(&bh_list.lock, flags);
}

#endif	/* __VIP_BUF_H__ */
