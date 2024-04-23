// SPDX-License-Identifier: GPL-2.0-or-later
#include <objtool/special.h>

bool arch_support_alt_relocation(struct special_alt *special_alt,
				 struct instruction *insn,
				 struct reloc *reloc)
{
	return false;
}

struct reloc *arch_find_switch_table(struct objtool_file *file,
				     struct instruction *insn)
{
	return NULL;
}

static inline bool check_addsub_u8(struct reloc *add, struct reloc * sub)
{
	return (reloc_type(add) == R_LARCH_ADD8) && (reloc_type(sub) == R_LARCH_SUB8) &&
	       (add->sym->sec == sub->sym->sec);
}

static inline unsigned long reloc_sec_off(struct reloc *reloc)
{
	return reloc->sym->offset + reloc_addend(reloc);
}

bool arch_set_group_len(unsigned int *orig_len, unsigned int *new_len, struct elf *elf,
			struct section *sec, unsigned long orig_off, unsigned long new_off)
{
	struct reloc *o = find_reloc_by_dest(elf, sec, orig_off);
	struct reloc *n = find_reloc_by_dest(elf, sec, new_off);

	if ((!o && n) || (o && !n))
		return false;

	if (!o && !n) {
		*orig_len = *(unsigned char *)(sec->data->d_buf + orig_off);
		*new_len = *(unsigned char *)(sec->data->d_buf + new_off);
		return true;
	}

	if (!check_addsub_u8(o - 1, o) || !check_addsub_u8(n - 1, n))
		return false;

	*orig_len = reloc_sec_off(o - 1) - reloc_sec_off(o);
	*new_len = reloc_sec_off(n - 1) - reloc_sec_off(n);
	return true;
}
