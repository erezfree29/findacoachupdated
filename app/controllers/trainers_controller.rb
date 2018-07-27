class TrainersController < ApplicationController
  skip_before_action :authenticate_user!

  def index
    @trainers = Trainer.all
 end

 def show
  @trainer = Trainer.find(params[:id])
  @expertise = Expertise.find(@trainer.expertise_id)

end

def new
  @user = current_user
  @trainer = Trainer.new()
  @trainer.user = @user
  @expertise = Expertise.all
end

def create
  @user = current_user
  @trainer = Trainer.new(trainer_params)
  @trainer.user = @user
  @expertise = Expertise.all

  if @trainer.save
    redirect_to trainer_path(@trainer)
  else
    render :new
  end

end

def edit


end

def update

end

def destroy

  @trainer = Trainer.find(params[:id])
  @trainer.destroy
  redirect_to trainers_path



end

private
def trainer_params

  params.require(:trainer).permit(:expertise_id, :hourly_rate, :photo, :address, :first_name,
    :last_name, :age, :gender)

 end
end
