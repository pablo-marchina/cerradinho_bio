import {useQuery} from '@tanstack/react-query';
import {api} from './api';
import {isMqttMode} from './runtime';
export const useSnapshot=()=>useQuery({queryKey:['snapshot'],queryFn:api.snapshot,staleTime:isMqttMode?250:1000,retry:1,refetchInterval:query=>isMqttMode?500:query.state.data?.commands?.some(c=>['created','published','received'].includes(c.status))?400:false});
