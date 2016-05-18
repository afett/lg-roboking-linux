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
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/interrupt.h>
#include <linux/videodev2.h>

#include "vip_buf.h"

#if (0)
#define DBGOUT(msg...)		{ printk(KERN_INFO "vip-buf: " msg); }
#else
#define DBGOUT(msg...)		do {} while (0)
#endif

#define ERROUT(msg...)		{ 	\
		printk(KERN_ERR "ERROR: %s, %s line %d: \n",	\
			__FILE__, __FUNCTION__, __LINE__),			\
		printk(KERN_ERR msg); }

/*--------------------------------------------------------------------------------
 * Globals
 */
static unsigned int		 buf_count = 0;
static unsigned int		 buf_alloc = 0;

struct buf_head 		 bh_list;
static struct vm_buffer* vmb_head  = NULL;
/*--------------------------------------------------------------------------------
 * Buffer manager
 */
extern int vmem_alloc(VM_IMEMORY *imem, int minor);
extern int vmem_free (VM_IMEMORY *imem);

int vip_buf_init(void)
{
	struct vm_buffer * vmb =
		kzalloc(sizeof(struct vm_buffer)*VIDEO_MAX_FRAME, GFP_KERNEL);
	if (! vmb) {
		ERROUT("fail, allocate video buffer ...\n");
		return -ENOMEM;
	}

	vmb_head  = vmb;
	buf_count = 0;
	buf_alloc = 0;
	return 0;
}

void vip_buf_exit(void)
{
	if (vmb_head)
		kfree(vmb_head);
	vmb_head = NULL;
}

int vip_buf_alloc(u_int fourcc, u_int width, u_int height,
				int halign, int valign, int count)
{
	struct vm_buffer *vmb = vmb_head;
	VM_VMEMORY *vm = NULL;
	int l_w, l_h, c_w, c_h;
	int i, minor = 0;
	u_int  flags = 0;

	DBGOUT("%s (fourcc=0x%x, w=%d, h=%d, h_a=%d, v_a=%d)\n",
		__func__, fourcc, width, height, halign, valign);

	if (count > VIDEO_MAX_FRAME) {
		ERROUT("fail, alloc buff %d out of range %d.n",
			count, VIDEO_MAX_FRAME);
		return -1;
	}

	if (buf_alloc)
		return 0;

	switch (fourcc) {
	case FOURCC_YUYV:	/* V4L2_PIX_FMT_YUYV */
		flags = VMEM_LINEAR_BUFFER;
		l_w = width<<1, l_h = height, c_w = 0, c_h = 0;
		break;
	case FOURCC_MVS0:
		flags = VMEM_BLOCK_BUFFER;
		l_w = width, l_h = height, c_w = l_w>>1, c_h = l_h>>1;
		break;
	case FOURCC_MVS2:
		flags = VMEM_BLOCK_BUFFER;
		l_w = width, l_h = height, c_w = l_w>>1, c_h = l_h;
		break;
	case FOURCC_MVS4:
		flags = VMEM_BLOCK_BUFFER;
		l_w = width, l_h = height, c_w = l_w, c_h = l_h;
		break;
	default:
		ERROUT("fail, not support fourcc (%c%c%c%c) \n",
			fourcc&0xFF,(fourcc>>8)&0xFF,(fourcc>>16)&0xFF,(fourcc>>24)&0xFF);
		return -EINVAL;
	}

	/* allocate video buffer */
	for (i = 0; count > i; i++, vmb++) {

		vm = &vmb->vm;

		/* lu */
		vm->LuMemory.Flags    = flags;
		vm->LuMemory.HorAlign = halign;
		vm->LuMemory.VerAlign = valign;
		vm->LuMemory.MemWidth = l_w;
		vm->LuMemory.MemHeight= l_h;

		if (l_w && l_h &&
			ERR_VMEM_NO != vmem_alloc(&vm->LuMemory, minor))
			goto _err;

		/* cb */
		vm->CbMemory.Flags    = flags;
		vm->CbMemory.HorAlign = halign;
		vm->CbMemory.VerAlign = valign;
		vm->CbMemory.MemWidth = c_w;
		vm->CbMemory.MemHeight= c_h;

		if (c_w && c_h &&
			ERR_VMEM_NO != vmem_alloc(&vm->CbMemory, minor))
			goto _err;

		/* cr */
		vm->CrMemory.Flags    = flags;
		vm->CrMemory.HorAlign = halign;
		vm->CrMemory.VerAlign = valign;
		vm->CrMemory.MemWidth = c_w;
		vm->CrMemory.MemHeight= c_h;

		if (c_w && c_h &&
			ERR_VMEM_NO != vmem_alloc(&vm->CrMemory, minor))
			goto _err;

		DBGOUT("%s, [%d] vmb=0x%08x, lu_a=0x%08x, cb_a=0x%08x, cr_a=0x%08x\n",
			__func__, i, (u_int)vmb,
			vm->LuMemory.Address, vm->CbMemory.Address, vm->CrMemory.Address);
	}

	init_vm_buf_head(&vmb_head->list, count);
	buf_alloc = 1;
	buf_count = count;
	return 0;

_err:
	for (; i >= 0; i--, vmb--) {
		vm = &vmb->vm;
		vmem_free(&vm->LuMemory);
		vmem_free(&vm->CbMemory);
		vmem_free(&vm->CrMemory);
	}

	buf_alloc = 0;
	return -ENOMEM;
}

void vip_buf_free(void)
{
	struct vm_buffer *vmb = vmb_head;
	VM_VMEMORY *vm = NULL;
	int i, cnt = buf_count;

	DBGOUT("%s\n", __func__);

	if (! vmb || ! buf_alloc)
		return;

	for (i = 0; cnt > i; i++, vmb++) {
		vm = &vmb->vm;
		vmem_free(&vm->LuMemory);
		vmem_free(&vm->CbMemory);
		vmem_free(&vm->CrMemory);
	}
	buf_alloc = 0;
}

struct vm_buffer* vip_buf_get_vm(int index)
{
	if (! buf_alloc)
		return NULL;
	return &vmb_head[index];
}

u_int vip_buf_get_phys(int index, int field)
{
	struct vm_buffer *vmb = &vmb_head[index];
	u_int phys = 0;

	if (! buf_alloc)
		return 0;

	switch (field) {
	case 0: phys = vmb->vm.LuMemory.Address; break;
	case 1: phys = vmb->vm.CbMemory.Address; break;
	case 2: phys = vmb->vm.CrMemory.Address; break;
	default:
		ERROUT("fail, invalid field\n");
		return 0;
	}
	return phys;
}

