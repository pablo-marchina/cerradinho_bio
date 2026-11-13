import {useQuery} from '@tanstack/react-query';
import {api} from './api';
export const useSnapshot=()=>useQuery({queryKey:['snapshot'],queryFn:api.snapshot,staleTime:1000,retry:1,refetchInterval:query=>query.state.data?.commands?.some(c=>['created','published','received'].includes(c.status))?400: false});
