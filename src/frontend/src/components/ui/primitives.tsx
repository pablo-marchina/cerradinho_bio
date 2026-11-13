import type { ButtonHTMLAttributes, HTMLAttributes } from 'react';
import { cva, type VariantProps } from 'class-variance-authority';
import { cn } from './utils';
const buttonVariants=cva('btn',{variants:{variant:{primary:'btn-primary',secondary:'btn-secondary',ghost:'btn-ghost'}},defaultVariants:{variant:'secondary'}});
export function Button({variant,className,...props}:ButtonHTMLAttributes<HTMLButtonElement>&VariantProps<typeof buttonVariants>){return <button type="button" className={cn(buttonVariants({variant}),className)} {...props}/>;}
export function Card({className,...props}:HTMLAttributes<HTMLDivElement>){return <div className={cn('card',className)} {...props}/>;}
