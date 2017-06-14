
#include "event.h"
#include <stdlib.h>
#include <stdio.h>

// to do: change to use the struct as parameter
bool predicate_result(struct predicate* pred)
{

	int* predicate_left = pred->predicate_left;
	int* predicate_right = pred->predicate_right;
	int* predicate_extra = pred->predicate_extra;
	int* predicate_extra2 = pred->predicate_extra2;
	int relation_num = pred->predicate_relation;

	switch (relation_num)
	{
	case 0:
		return *predicate_left < *predicate_right ? true : false;
	case 1:
		return *predicate_left == *predicate_right ? true : false;
	case 2:
		return *predicate_left != *predicate_right ? true : false;
	case 3:
		return true;
	case 4:
		return (*predicate_extra == 0) && (*predicate_left == *predicate_right - 1) && (*predicate_left < BLOCK_LAST_rule1) ? true : false;
	case 5:
		return (*predicate_left == *predicate_right) && (*predicate_left < BLOCK_LAST_rule1) ? true : false;
	case 6:
		return *predicate_left >= *predicate_right ? true : false;
	case 7:
		return *predicate_left <= *predicate_right ? true : false;
	case 8:
		return (*predicate_left == *predicate_right) && (*predicate_left == BLOCK_LAST_rule1) ? true : false;
	case 9:
		return (*predicate_extra == 0) && (*predicate_left == *predicate_right - 1) && (*predicate_left == BLOCK_LAST_rule1) ? true : false;
	case 10:
		return (*predicate_extra == 1) || (*predicate_extra2 == 0 && ((*predicate_left == 0 && *predicate_left != *predicate_right) || *predicate_left < *predicate_right - 1)) ? true : false;
	case 11:
		return (*predicate_extra == 0) && (*predicate_left == *predicate_right) ? true : false;
	case 12:
		return (*predicate_extra == 0) ? true : false;
	case 13:
		return (*predicate_extra == 1) ? true : false;
	case 14:
		return (*predicate_extra == 0) && ((*predicate_left != 0  && *predicate_left == *predicate_right - 1) || *predicate_extra2 == 1) ? true : false;
	case 15:
		return (*predicate_left != *predicate_right) && (*predicate_extra == 0) ? true : false;
	case 16:
		return (*predicate_left != *predicate_right) && (*predicate_extra == 1) ? true : false;
	case 17:
		return (*predicate_extra == 1) && (*predicate_left != *predicate_right) ? true : false;
	case 18:
		return (*predicate_extra == 1) || (*predicate_left >= *predicate_right) ? true : false;
	case 19:
		return (*predicate_extra == 0) && (*predicate_left < *predicate_right) ? true : false;
	case 20:
		return (*predicate_extra == 1) || (*predicate_left < *predicate_right) ? true : false;
	case 21:
		return (*predicate_extra == 0) && (*predicate_left == *predicate_right) && (*predicate_left < BLOCK_LAST_rule1)  ? true : false;
	case 22:
		return (*predicate_extra == 0) && (*predicate_left == *predicate_right) && (*predicate_left == BLOCK_LAST_rule1) ? true : false;
	case 23:
		return (*predicate_extra == 1) || (*predicate_left == 0) ? true : false;
	case 24:
		return *predicate_left != 0 ? true : false;
	case 25:
		return (*predicate_extra == 0) && (*predicate_left != 0) ? true : false;
	case 26:
		return *predicate_left == (1 << *predicate_right) ? true : false;
	}
	return false;
}

void update_action(struct assignment* assign)
{
	//  printf("before: left:%d, right:%d\n",*assignment_left, *assignment_right);
	int* assignment_left = assign->assignment_left;
	int* assignment_right = assign->assignment_right;
	int relation_num = assign->assignment_relation;

	switch (relation_num)
	{
	case 0:
		*assignment_left = *assignment_right + 1;
		break;
	case 1:
		*assignment_left = 0;
		break;
	case 2:
		*assignment_left = 1;
		break;
	case 3:
		*assignment_left = *assignment_right;
		break;

		// it may have other advanced relation like left= right+10 later;
	}
	//   printf("after: left:%d, right:%d\n",*assignment_left, *assignment_right);
	return ;
}



bool final_check(struct Rule_EEFSM* rule)
{

	if (rule->current_state_id == rule->accept_state_id)
		return true;
	else
		return false;


}


// checker algorithm
bool update_check(struct Rule_EEFSM* rule, struct Event_FSM triggered_event, char* buf)
{

	// Sometimes the dispacted event by scheduler is not the defined event in EEFSM
	//if (!if_event_in_FSM(rule, triggered_event));  return true;
	//printf("Update: current_state_id: %d, accept state: %d \n", rule->current_state_id,rule->accept_state_id);

	//      return true;
	rule->current_event = triggered_event;
	int i = 0, match = 0, assgin_update = 0;

	rule->next_state_id = 0;
	struct assignment* tmp_assign;               //for update action use



	while (rule->states[rule->current_state_id].outgoing[i].to_state_id != 0 ) //for each active transition in current_state
	{
		//printf("from_state_id: %d \n", i);
		if (rule->states[rule->current_state_id].outgoing[i].event_flag == triggered_event.flag)
		{

			if (predicate_result(&rule->states[rule->current_state_id].outgoing[i].pred))
			{

				rule->next_state_id = rule->states[rule->current_state_id].outgoing[i].to_state_id;

				//printf("match: outgoing i :%d, next state: %d, machine var 1: %d \n",i, rule->next_state_id,rule->machine_var[1]);
				// printf("(%d,%d,%d)", rule->current_state_id, i,rule->next_state_id);
				//int tmp=strlen(buf);
				//sprintf(buf+tmp,"%d%d%d,",rule->current_state_id, i,rule->next_state_id);

				if (rule->states[rule->current_state_id].outgoing[i].assign.assignment_left != NULL)
				{


					//printf("left: %p, right:%p, relation %d",tmp2_left, tmp2_right, tmp2_relation );
					tmp_assign = &rule->states[rule->current_state_id].outgoing[i].assign;

					assgin_update = 1;


				}
				// update values after event triggered


				match++;
				//printf("match:%d\n", match);
				if (match > 1 )
				{
					int tmp = strlen(buf);
					sprintf(buf + tmp, "%s", "match more than 1");
					return false;
				}

			}

		}
		i++;
	}

	if (rule->next_state_id == 0)
	{
		int tmp = strlen(buf);
		sprintf(buf + tmp, "no matching:event %d with %d in S%d", triggered_event.flag, triggered_event.param, rule->current_state_id);
		return false;
	}
	else
	{
		if (assgin_update == 1)
		{
			update_action(tmp_assign);
		}

		rule->current_state_id = rule->next_state_id; //does not include the current state
		/*
		        int length=strlen(rule->history_sequence);
		        sprintf(rule->history_sequence+length,"%d:",rule->current_state_id);
		        rule->current_state_id=rule->next_state_id; //does not include the current state

		           for (i=1; i<= rule->sizeOfvar;i++){ // for macchine variables
		           if(i==5) continue; //var[5] always =MAX_TIMEOUTS;
		           length +=2;
		           sprintf(rule->history_sequence+length, "%d,", rule->machine_var[i]);
		           }
		*/

		//printf("history:%s\n",rule->history_sequence);

		return true;
	}

}


